#ifndef MERCURY_AUTONOMY__ACTION_SERVER_H_
#define MERCURY_AUTONOMY__ACTION_SERVER_H_

#include <memory>
#include <string>
#include <map>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <behaviortree_cpp/bt_factory.h>

#include "autonomy_core.h"
#include "tree_manager.h"

namespace mercury_autonomy
{

// Placeholder action for now - replace with your actual action type
// This would typically be something like mercury_autonomy_msgs::action::RunTree
struct RunTreeAction
{
  struct Goal
  {
    std::string tree_name;
  };
  struct Result
  {
    bool success;
    std::string message;
  };
  struct Feedback
  {
    std::string current_status;
  };
};

using RunTreeActionHandle = rclcpp_action::ServerGoalHandle<RunTreeAction>;

/**
 * @class AutonomyActionServer
 * @brief ROS2 action server for executing behavior trees
 *
 * This class provides a ROS2 action server interface for requesting the execution
 * of behavior trees. It manages the lifecycle of tree execution and reports feedback
 * and results back to clients.
 */
class AutonomyActionServer : public rclcpp::Node
{
public:
  /**
   * @brief Constructor for AutonomyActionServer
   * @param node_name Name of the ROS2 node
   * @param action_name Name of the action server
   */
  AutonomyActionServer(
    const std::string & node_name = "autonomy_action_server",
    const std::string & action_name = "run_tree");

  /**
   * @brief Initialize the action server
   * @return True if initialization was successful
   */
  bool initialize();

protected:
  std::shared_ptr<AutonomyCore> autonomy_core_;
  std::shared_ptr<TreeManager> tree_manager_;

  // Placeholder for action server - would be: rclcpp_action::Server<RunTreeAction>::SharedPtr
  // Keeping as comment until actual action definition is created
  // rclcpp_action::Server<RunTreeAction>::SharedPtr action_server_;

  std::string action_name_;

  /**
   * @brief Handle goal requests from action clients
   * @param uuid The goal ID
   * @param goal The goal request
   * @return The policy for accepting/rejecting the goal
   */
  rclcpp_action::GoalResponse handle_goal(
    const rclcpp_action::GoalUUID & uuid,
    std::shared_ptr<const RunTreeAction::Goal> goal);

  /**
   * @brief Handle cancellation requests
   * @param goal_handle The goal handle to cancel
   * @return The policy for accepting/rejecting the cancellation
   */
  rclcpp_action::CancelResponse handle_cancel(
    const std::shared_ptr<RunTreeActionHandle> goal_handle);

  /**
   * @brief Execute the goal (run the behavior tree)
   * @param goal_handle The goal handle
   */
  void handle_accepted(const std::shared_ptr<RunTreeActionHandle> goal_handle);
};

/**
 * @brief Create an AutonomyActionServer as a ROS2 component
 */
class AutonomyComponent : public AutonomyActionServer
{
public:
  explicit AutonomyComponent(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());
};

}  // namespace mercury_autonomy

#endif  // MERCURY_AUTONOMY__ACTION_SERVER_H_
