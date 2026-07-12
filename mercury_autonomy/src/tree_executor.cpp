// Tree executor -- loads and ticks BT XML files via a ROS2 action server.
//
// Provides:
//   - autonomy/execute_tree (ExecuteTree action server)  -- starts and monitors tree execution.
//   - autonomy/list_trees   (std_srvs/Trigger service)   -- enumerates available tree XML files.
//   - autonomy/status       (std_msgs/String publisher)   -- current execution status string.
//
// The action server handles goal acceptance/rejection, cancellation, feedback, and results.
// Only one tree may execute at a time; concurrent goals are rejected.

#include "mercury_autonomy/autonomy_lib.hpp"

#include <atomic>
#include <chrono>
#include <filesystem>
#include <mutex>
#include <thread>

#include <mercury_msgs/action/execute_tree.hpp>
#include <std_msgs/msg/string.hpp>
#include <mercury_msgs/srv/list_trees.hpp>

#ifndef AUTONOMY_PKG_NAME
#define AUTONOMY_PKG_NAME "mercury_autonomy"
#endif

using namespace std::chrono_literals;
using namespace std::placeholders;

namespace mercury_autonomy
{

// Result status codes matching ExecuteTree.action
constexpr int32_t RESULT_SUCCESS = 0;
constexpr int32_t RESULT_FAILURE = 1;
constexpr int32_t RESULT_CANCELED = 2;
constexpr int32_t RESULT_ERROR = 3;

using ExecuteTree = mercury_msgs::action::ExecuteTree;
using GoalHandle = rclcpp_action::ServerGoalHandle<ExecuteTree>;

/// The tree executor node -- ROS2 action server for BehaviorTree execution.
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

    // Action server: execute_tree (replaces the old topic-based trigger)
    action_server_ = rclcpp_action::create_server<ExecuteTree>(
      this,
      "autonomy/execute_tree",
      std::bind(&TreeExecutor::handleGoal, this, _1, _2),
      std::bind(&TreeExecutor::handleCancel, this, _1),
      std::bind(&TreeExecutor::handleAccepted, this, _1));

    // Service: list available tree XML files
    list_trees_srv_ = create_service<mercury_msgs::srv::ListTrees>(
      "autonomy/list_trees",
      std::bind(&TreeExecutor::handleListTrees, this, _1, _2));

    // Status publisher (tree state as string for debugging / monitoring)
    status_pub_ = create_publisher<std_msgs::msg::String>("autonomy/current_status", 10);

    RCLCPP_INFO(
      get_logger(), "TreeExecutor ready (tick rate: %.1f Hz). "
      "Searching trees in %zu directories.",
      tick_rate_hz_, tree_dirs_.size());
  }

  ~TreeExecutor() override
  {
    // Ensure clean shutdown if a tree is still running
    cancel_requested_.store(true);
    if (exec_thread_.joinable()) {
      exec_thread_.join();
    }
  }

private:
  // ---------------------------------------------------------------------------
  // Action server callbacks
  // ---------------------------------------------------------------------------

  /// Decide whether to accept or reject a new goal.
  rclcpp_action::GoalResponse handleGoal(
    const rclcpp_action::GoalUUID & /*uuid*/,
    std::shared_ptr<const ExecuteTree::Goal> goal)
  {
    RCLCPP_INFO(get_logger(), "Received goal: tree_path='%s'", goal->tree_path.c_str());

    // Reject if a tree is already running
    if (tree_running_.load()) {
      RCLCPP_WARN(get_logger(), "A tree is already running -- rejecting goal.");
      return rclcpp_action::GoalResponse::REJECT;
    }

    // Resolve relative paths against the known tree directories
    auto resolved = resolveTreePath(goal->tree_path);
    if (resolved.empty()) {
      RCLCPP_ERROR(get_logger(), "Tree file not found: %s", goal->tree_path.c_str());
      return rclcpp_action::GoalResponse::REJECT;
    }

    // Store resolved path for the execution thread
    resolved_tree_path_ = resolved;
    RCLCPP_INFO(get_logger(), "Resolved tree path: %s", resolved.c_str());
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  /// Handle a cancellation request for a running goal.
  rclcpp_action::CancelResponse handleCancel(
    const std::shared_ptr<GoalHandle>/*goal_handle*/)
  {
    RCLCPP_INFO(get_logger(), "Cancel requested for running tree.");
    cancel_requested_.store(true);
    return rclcpp_action::CancelResponse::ACCEPT;
  }

  /// Called when a goal is accepted -- launch tree execution on a background thread.
  void handleAccepted(const std::shared_ptr<GoalHandle> goal_handle)
  {
    // Join any previous execution thread before starting a new one
    if (exec_thread_.joinable()) {
      exec_thread_.join();
    }

    exec_thread_ = std::thread(
      [this, goal_handle]() {
        executeTree(goal_handle);
      });
  }

  // ---------------------------------------------------------------------------
  // Tree execution (runs on background thread)
  // ---------------------------------------------------------------------------

  /// Execute the BT from the resolved tree path.  Publishes feedback and sets the result.
  void executeTree(const std::shared_ptr<GoalHandle> goal_handle)
  {
    const auto tree_path = resolved_tree_path_;
    RCLCPP_INFO(get_logger(), "Starting tree execution: %s", tree_path.c_str());

    tree_running_.store(true);
    cancel_requested_.store(false);
    publishStatus("LOADING");

    auto result = std::make_shared<ExecuteTree::Result>();
    auto feedback = std::make_shared<ExecuteTree::Feedback>();
    const auto start_time = std::chrono::steady_clock::now();

    try {
      BT::Tree tree = factory_->createTreeFromFile(tree_path);
      initRosForTree(tree, shared_from_this());

      publishStatus("RUNNING");
      feedback->current_status = "RUNNING";
      feedback->stack.stack = {tree_path};
      feedback->stack.node_id = 0;

      // Tick loop at configured rate
      rclcpp::Rate loop_rate{tick_rate_hz_};
      auto bt_status = BT::NodeStatus::RUNNING;

      while (bt_status == BT::NodeStatus::RUNNING && rclcpp::ok()) {
        // Check for cancellation
        if (goal_handle->is_canceling() || cancel_requested_.load()) {
          RCLCPP_INFO(get_logger(), "Canceling tree execution.");
          tree.haltTree();

          result->return_code = RESULT_CANCELED;

          MercuryBtNode::staticDeinit();
          tree_running_.store(false);
          cancel_requested_.store(false);
          publishStatus("CANCELED");
          goal_handle->canceled(result);
          return;
        }

        bt_status = tree.tickOnce();

        // Publish feedback with elapsed time
        feedback->elapsed_seconds = elapsedSeconds(start_time);
        feedback->current_status = BT::toStr(bt_status);
        feedback->stack.stack = {tree_path, std::string(BT::toStr(bt_status))};
        feedback->stack.node_id = 0;
        goal_handle->publish_feedback(feedback);

        loop_rate.sleep();
      }

      // Tree finished naturally
      MercuryBtNode::staticDeinit();
      tree_running_.store(false);

      if (bt_status == BT::NodeStatus::SUCCESS) {
        result->return_code = RESULT_SUCCESS;
      } else {
        result->return_code = RESULT_FAILURE;
      }

      publishStatus(BT::toStr(bt_status));
      const auto elapsed = elapsedSeconds(start_time);
      RCLCPP_INFO(
        get_logger(), "Tree finished: %s (%.2fs)",
        BT::toStr(bt_status).c_str(), elapsed);
      goal_handle->succeed(result);

    } catch (const std::exception & e) {
      RCLCPP_ERROR(get_logger(), "Tree execution error: %s", e.what());
      finishWithError(goal_handle, e.what());
    } catch (...) {
      RCLCPP_ERROR(get_logger(), "Unknown error during tree execution.");
      finishWithError(goal_handle, "Unknown error");
    }
  }

  /// Helper: abort goal with ERROR status after an exception.
  void finishWithError(
    const std::shared_ptr<GoalHandle> & goal_handle,
    const std::string & error_msg)
  {
    MercuryBtNode::staticDeinit();
    tree_running_.store(false);
    cancel_requested_.store(false);
    publishStatus("ERROR");

    auto result = std::make_shared<ExecuteTree::Result>();
    result->return_code = RESULT_ERROR;
    RCLCPP_ERROR(get_logger(), "Tree execution aborted with ERROR: %s", error_msg.c_str());
    goal_handle->abort(result);
  }

  // ---------------------------------------------------------------------------
  // Service callbacks
  // ---------------------------------------------------------------------------

  /// Enumerate all .xml tree files across configured directories.
  void handleListTrees(
    const mercury_msgs::srv::ListTrees::Request::SharedPtr /*request*/,
    mercury_msgs::srv::ListTrees::Response::SharedPtr response)
  {
    std::vector<std::string> tree_files;
    for (const auto & dir : tree_dirs_) {
      if (!std::filesystem::is_directory(dir)) {
        continue;
      }
      for (const auto & entry : std::filesystem::directory_iterator(dir)) {
        const std::string path = entry.path().string();
        if (path.size() >= 4 && path.substr(path.size() - 4) == ".xml") {
          tree_files.push_back(path);
        }
      }
    }
    response->trees = tree_files;
  }

  // ---------------------------------------------------------------------------
  // Utility
  // ---------------------------------------------------------------------------

  void publishStatus(const std::string & status)
  {
    std_msgs::msg::String msg;
    msg.data = status;
    status_pub_->publish(msg);
  }

  /// Resolve a tree path: absolute paths are used directly, relative names are
  /// searched in the configured tree directories.  Returns empty string if not found.
  std::string resolveTreePath(const std::string & input) const
  {
    // Absolute path -- use as-is if it exists
    if (!input.empty() && input[0] == '/') {
      return std::filesystem::exists(input) ? input : std::string{};
    }

    // Relative -- look in each tree directory
    for (const auto & dir : tree_dirs_) {
      auto candidate = std::filesystem::path(dir) / input;
      if (std::filesystem::exists(candidate)) {
        return candidate.string();
      }
    }
    return {};
  }

  /// Compute seconds elapsed since a reference time point.
  static double elapsedSeconds(const std::chrono::steady_clock::time_point & start)
  {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(now - start).count();
  }

  // ---------------------------------------------------------------------------
  // Members
  // ---------------------------------------------------------------------------
  std::shared_ptr<BT::BehaviorTreeFactory> factory_;
  std::vector<std::string> tree_dirs_;
  std::vector<std::string> extra_tree_dirs_;
  std::vector<std::string> extra_plugins_;
  double tick_rate_hz_ = 30.0;

  std::atomic<bool> tree_running_{false};
  std::atomic<bool> cancel_requested_{false};
  std::string resolved_tree_path_;  ///< Set by handleGoal, read by executeTree
  std::thread exec_thread_;

  rclcpp_action::Server<ExecuteTree>::SharedPtr action_server_;
  rclcpp::Service<mercury_msgs::srv::ListTrees>::SharedPtr list_trees_srv_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_pub_;
};

}  // namespace mercury_autonomy


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<mercury_autonomy::TreeExecutor>();

  RCLCPP_INFO(node->get_logger(), "TreeExecutor node started.");

  // MultiThreadedExecutor is required so the action server can process
  // cancel requests while the tree execution thread is running.
  rclcpp::executors::MultiThreadedExecutor executor;
  executor.add_node(node);
  executor.spin();

  node.reset();
  rclcpp::shutdown();
  return 0;
}
