// Tree executor -- a ROS2 action server that loads and ticks BT XML files.
//
// Provides:
//   - An action server (autonomy/execute_tree) to run a tree by file path.
//   - A service server (autonomy/list_trees) to enumerate available trees.
//
// This is the mercury_autonomy equivalent of riptide_autonomy's DoTask node,
// rebuilt for BT.CPP v4 without riptide-specific message dependencies.

#include "mercury_autonomy/autonomy_lib.hpp"

#include <filesystem>
#include <thread>

#include <std_msgs/msg/string.hpp>
#include <std_srvs/srv/trigger.hpp>

#ifndef AUTONOMY_PKG_NAME
#define AUTONOMY_PKG_NAME "mercury_autonomy"
#endif

using namespace std::chrono_literals;
using namespace std::placeholders;

namespace mercury_autonomy
{

// Action definition -- uses a simple string goal / int result.
// Until a project-specific action type is created, we encode
// tree path as a Trigger-like request and return status via
// a std_msgs/String feedback.  A custom action interface should
// replace this once mercury_msgs (or equivalent) is available.
//
// For now the executor exposes only the list_trees service and
// a minimal execute_tree action (goal = tree path string, result = int).

/// The tree executor node.
class TreeExecutor : public rclcpp::Node
{
public:
  TreeExecutor()
  : Node("tree_executor")
  {
    // Declare parameters
    declare_parameter<std::string>("tree_directory", "");
    declare_parameter<std::vector<std::string>>("extra_tree_dirs", std::vector<std::string>());
    declare_parameter<std::vector<std::string>>("extra_plugins", std::vector<std::string>());

    // Read parameters
    extra_tree_dirs_ = get_parameter("extra_tree_dirs").as_string_array();
    extra_plugins_ = get_parameter("extra_plugins").as_string_array();

    std::string param_tree_dir = get_parameter("tree_directory").as_string();
    if (!param_tree_dir.empty()) {
      tree_dirs_.push_back(param_tree_dir);
    }

    // Default tree directories: install share and source-local
    tree_dirs_.push_back(
      ament_index_cpp::get_package_share_directory(AUTONOMY_PKG_NAME) + "/trees");
    tree_dirs_.insert(tree_dirs_.end(), extra_tree_dirs_.begin(), extra_tree_dirs_.end());

    // Build the BT factory and register our plugins
    factory_ = std::make_shared<BT::BehaviorTreeFactory>();
    RCLCPP_INFO(get_logger(), "Registering core autonomy plugins");
    registerPlugins(factory_, AUTONOMY_PKG_NAME);

    // Load additional plugin shared libraries from parameter
    for (const auto & plugin_path : extra_plugins_) {
      try {
        RCLCPP_INFO(get_logger(), "Loading extra plugin: %s", plugin_path.c_str());
        factory_->registerFromPlugin(plugin_path);
      } catch (const BT::RuntimeError & e) {
        RCLCPP_ERROR(
          get_logger(), "Failed to load plugin %s: %s",
          plugin_path.c_str(), e.what());
      }
    }

    // Service: list available tree XML files
    list_trees_srv_ = create_service<std_srvs::srv::Trigger>(
      "autonomy/list_trees",
      std::bind(&TreeExecutor::handleListTrees, this, _1, _2));

    // Feedback publisher (tree status as string for debugging)
    status_pub_ = create_publisher<std_msgs::msg::String>("autonomy/status", 10);

    RCLCPP_INFO(
      get_logger(), "TreeExecutor ready. Searching trees in %zu directories.",
      tree_dirs_.size());
  }

  /// Execute a tree by file path. Call this from an external trigger
  /// (action client, service, etc.) once a proper action type is defined.
  /// Returns the BT::NodeStatus as an integer.
  int executeTree(const std::string & tree_path)
  {
    if (tree_running_) {
      RCLCPP_WARN(get_logger(), "A tree is already running -- rejecting request.");
      return -1;
    }

    if (!std::filesystem::exists(tree_path)) {
      RCLCPP_ERROR(get_logger(), "Tree file does not exist: %s", tree_path.c_str());
      return -1;
    }

    tree_running_ = true;
    publishStatus("LOADING");

    try {
      BT::Tree tree = factory_->createTreeFromFile(tree_path);
      initRosForTree(tree, shared_from_this());

      publishStatus("RUNNING");
      rclcpp::Rate loop_rate(30ms);
      auto status = BT::NodeStatus::RUNNING;

      while (status == BT::NodeStatus::RUNNING && rclcpp::ok()) {
        status = tree.tickOnce();
        loop_rate.sleep();
      }

      tree_running_ = false;
      publishStatus(BT::toStr(status));
      RCLCPP_INFO(get_logger(), "Tree finished with status: %s", BT::toStr(status).c_str());
      return static_cast<int>(status);

    } catch (const std::exception & e) {
      RCLCPP_ERROR(get_logger(), "Tree execution error: %s", e.what());
    } catch (...) {
      RCLCPP_ERROR(get_logger(), "Unknown error during tree execution.");
    }

    tree_running_ = false;
    publishStatus("ERROR");
    return -1;
  }

private:
  // Enumerate all .xml tree files across configured directories.
  void handleListTrees(
    const std_srvs::srv::Trigger::Request::SharedPtr /*request*/,
    std_srvs::srv::Trigger::Response::SharedPtr response)
  {
    std::string listing;
    for (const auto & dir : tree_dirs_) {
      if (!std::filesystem::is_directory(dir)) {
        continue;
      }
      for (const auto & entry : std::filesystem::directory_iterator(dir)) {
        const std::string path = entry.path().string();
        if (path.size() >= 4 && path.substr(path.size() - 4) == ".xml") {
          listing += path + "\n";
        }
      }
    }
    response->success = true;
    response->message = listing;
  }

  void publishStatus(const std::string & status)
  {
    std_msgs::msg::String msg;
    msg.data = status;
    status_pub_->publish(msg);
  }

  // Members
  std::shared_ptr<BT::BehaviorTreeFactory> factory_;
  std::vector<std::string> tree_dirs_;
  std::vector<std::string> extra_tree_dirs_;
  std::vector<std::string> extra_plugins_;
  bool tree_running_ = false;

  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr list_trees_srv_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_pub_;
};

}  // namespace mercury_autonomy


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<mercury_autonomy::TreeExecutor>();

  RCLCPP_INFO(node->get_logger(), "TreeExecutor node started.");
  rclcpp::executors::MultiThreadedExecutor executor;
  executor.add_node(node);
  executor.spin();

  rclcpp::shutdown();
  return 0;
}
