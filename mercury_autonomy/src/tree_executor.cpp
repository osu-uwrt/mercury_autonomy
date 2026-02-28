// Tree executor -- loads and ticks BT XML files from ROS2 triggers.
//
// Provides:
//   - autonomy/execute_tree (std_msgs/String subscriber) -- starts a tree by path.
//   - autonomy/cancel_tree  (std_srvs/Trigger service)   -- cancels the running tree.
//   - autonomy/list_trees   (std_srvs/Trigger service)   -- enumerates available trees.
//   - autonomy/status       (std_msgs/String publisher)   -- current execution status.
//
// A formal ROS2 action server (with goal/feedback/result) should replace the
// topic-based trigger once mercury_msgs defines a custom action type.
// The cancel + status interfaces intentionally mirror the action server pattern
// so the migration will be transparent to upstream callers.

#include "mercury_autonomy/autonomy_lib.hpp"

#include <atomic>
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
    declare_parameter<double>("tick_rate_hz", 30.0);

    // Read parameters
    extra_tree_dirs_ = get_parameter("extra_tree_dirs").as_string_array();
    extra_plugins_ = get_parameter("extra_plugins").as_string_array();
    tick_rate_hz_ = get_parameter("tick_rate_hz").as_double();

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

    // Subscriber: receive tree path to execute
    execute_tree_sub_ = create_subscription<std_msgs::msg::String>(
      "autonomy/execute_tree", 1,
      std::bind(&TreeExecutor::handleExecuteTree, this, _1));

    // Service: cancel a running tree
    cancel_tree_srv_ = create_service<std_srvs::srv::Trigger>(
      "autonomy/cancel_tree",
      std::bind(&TreeExecutor::handleCancelTree, this, _1, _2));

    // Service: list available tree XML files
    list_trees_srv_ = create_service<std_srvs::srv::Trigger>(
      "autonomy/list_trees",
      std::bind(&TreeExecutor::handleListTrees, this, _1, _2));

    // Status publisher (tree state as string for debugging / monitoring)
    status_pub_ = create_publisher<std_msgs::msg::String>("autonomy/status", 10);

    RCLCPP_INFO(
      get_logger(), "TreeExecutor ready (tick rate: %.1f Hz). "
      "Searching trees in %zu directories.",
      tick_rate_hz_, tree_dirs_.size());
  }

  ~TreeExecutor() override
  {
    // Ensure clean shutdown if a tree is still running
    cancelTree();
    if (exec_thread_.joinable()) {
      exec_thread_.join();
    }
  }

  /// Cancel the currently running tree (thread-safe).
  void cancelTree()
  {
    cancel_requested_.store(true);
  }

  /// Execute a tree by file path. Runs on a background thread.
  /// Returns the BT::NodeStatus as an integer.
  int executeTree(const std::string & tree_path)
  {
    if (tree_running_.load()) {
      RCLCPP_WARN(get_logger(), "A tree is already running -- rejecting request.");
      return -1;
    }

    if (!std::filesystem::exists(tree_path)) {
      RCLCPP_ERROR(get_logger(), "Tree file does not exist: %s", tree_path.c_str());
      return -1;
    }

    tree_running_.store(true);
    cancel_requested_.store(false);
    publishStatus("LOADING");

    try {
      BT::Tree tree = factory_->createTreeFromFile(tree_path);
      initRosForTree(tree, shared_from_this());

      publishStatus("RUNNING");

      // Tick loop at configured rate
      rclcpp::Rate loop_rate{tick_rate_hz_};
      auto status = BT::NodeStatus::RUNNING;

      while (status == BT::NodeStatus::RUNNING && rclcpp::ok()) {
        // Check for external cancel request
        if (cancel_requested_.load()) {
          RCLCPP_INFO(get_logger(), "Cancel requested -- halting tree.");
          tree.haltTree();
          status = BT::NodeStatus::IDLE;
          break;
        }

        status = tree.tickOnce();
        loop_rate.sleep();
      }

      // Release static resources (TF buffer/listener) to avoid leaks
      MercuryBtNode::staticDeinit();

      tree_running_.store(false);
      cancel_requested_.store(false);
      publishStatus(BT::toStr(status));
      RCLCPP_INFO(get_logger(), "Tree finished with status: %s", BT::toStr(status).c_str());
      return static_cast<int>(status);

    } catch (const std::exception & e) {
      RCLCPP_ERROR(get_logger(), "Tree execution error: %s", e.what());
    } catch (...) {
      RCLCPP_ERROR(get_logger(), "Unknown error during tree execution.");
    }

    MercuryBtNode::staticDeinit();
    tree_running_.store(false);
    cancel_requested_.store(false);
    publishStatus("ERROR");
    return -1;
  }

private:
  // Subscriber callback: launch tree execution in a background thread.
  void handleExecuteTree(const std_msgs::msg::String::SharedPtr msg)
  {
    if (tree_running_.load()) {
      RCLCPP_WARN(get_logger(), "A tree is already running -- ignoring request.");
      return;
    }

    // Join any previous execution thread
    if (exec_thread_.joinable()) {
      exec_thread_.join();
    }

    const std::string path = msg->data;
    RCLCPP_INFO(get_logger(), "Execute tree request: %s", path.c_str());

    exec_thread_ = std::thread(
      [this, path]() {
        this->executeTree(path);
      });
  }

  // Service callback: cancel the running tree.
  void handleCancelTree(
    const std_srvs::srv::Trigger::Request::SharedPtr /*request*/,
    std_srvs::srv::Trigger::Response::SharedPtr response)
  {
    if (!tree_running_.load()) {
      response->success = false;
      response->message = "No tree is currently running.";
      return;
    }
    cancelTree();
    response->success = true;
    response->message = "Cancel requested.";
  }

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
  double tick_rate_hz_ = 30.0;

  std::atomic<bool> tree_running_{false};
  std::atomic<bool> cancel_requested_{false};
  std::thread exec_thread_;

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr execute_tree_sub_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr cancel_tree_srv_;
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

  // Clean shutdown
  node.reset();
  rclcpp::shutdown();
  return 0;
}
