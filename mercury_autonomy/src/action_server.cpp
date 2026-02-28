#include "mercury_autonomy/action_server.h"
#include <rclcpp/rclcpp.hpp>

namespace mercury_autonomy
{

AutonomyActionServer::AutonomyActionServer(
  const std::string & node_name,
  const std::string & action_name)
: rclcpp::Node(node_name), action_name_(action_name)
{
}

bool AutonomyActionServer::initialize()
{
  try {
    // Initialize the autonomy core
    autonomy_core_ = std::make_shared<AutonomyCore>(shared_from_this());
    if (!autonomy_core_->initialize()) {
      RCLCPP_ERROR(get_logger(), "Failed to initialize AutonomyCore");
      return false;
    }

    // Initialize the tree manager
    double tick_rate = declare_parameter<double>("tick_rate", 10.0);
    tree_manager_ = std::make_shared<TreeManager>(shared_from_this(), tick_rate);

    // TODO: Create the actual action server
    // action_server_ = rclcpp_action::create_server<RunTreeAction>(
    //   shared_from_this(),
    //   action_name_,
    //   std::bind(&AutonomyActionServer::handle_goal, this, std::placeholders::_1,
    //   std::placeholders::_2),
    //   std::bind(&AutonomyActionServer::handle_cancel, this, std::placeholders::_1),
    //   std::bind(&AutonomyActionServer::handle_accepted, this, std::placeholders::_1)
    // );

    RCLCPP_INFO(get_logger(), "AutonomyActionServer initialized successfully");
    return true;
  } catch (const std::exception & e) {
    RCLCPP_ERROR(get_logger(), "Failed to initialize AutonomyActionServer: %s", e.what());
    return false;
  }
}

rclcpp_action::GoalResponse AutonomyActionServer::handle_goal(
  const rclcpp_action::GoalUUID & uuid,
  std::shared_ptr<const RunTreeAction::Goal> goal)
{
  RCLCPP_INFO(
    get_logger(), "Received goal request to run tree: %s", goal->tree_name.c_str());

  // Accept all goals for now
  (void)uuid;
  return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse AutonomyActionServer::handle_cancel(
  const std::shared_ptr<RunTreeActionHandle> goal_handle)
{
  RCLCPP_INFO(get_logger(), "Received cancel request");
  (void)goal_handle;
  return rclcpp_action::CancelResponse::ACCEPT;
}

void AutonomyActionServer::handle_accepted(
  const std::shared_ptr<RunTreeActionHandle> goal_handle)
{
  RCLCPP_INFO(get_logger(), "Executing goal");
  (void)goal_handle;

  // TODO: Implement goal execution logic
  // This is where the tree would be loaded and executed
}

AutonomyComponent::AutonomyComponent(const rclcpp::NodeOptions & options)
: AutonomyActionServer("autonomy_component", "run_tree")
{
  (void)options;
  initialize();
}

}  // namespace mercury_autonomy
