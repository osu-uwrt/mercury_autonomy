#include "mercury_autonomy/bt_actions/apply_feedforward_force.hpp"
#include <algorithm>
#include <cctype>

namespace mercury_autonomy
{

void ApplyFeedforwardForce::rosInit()
{
  pub_ = rosNode()->create_publisher<mercury_msgs::msg::ControllerCommand>("controller/linear", 10);
}

BT::NodeStatus ApplyFeedforwardForce::onStart()
{
  std::string axis;
  double force = 0.0;
  double duration = 0.0;

  if (!getInput("axis", axis)) {
    RCLCPP_ERROR(rosNode()->get_logger(), "ApplyFeedforwardForce: missing required input [axis]");
    return BT::NodeStatus::FAILURE;
  }
  if (!getInput("force", force)) {
    RCLCPP_ERROR(rosNode()->get_logger(), "ApplyFeedforwardForce: missing required input [force]");
    return BT::NodeStatus::FAILURE;
  }
  if (!getInput("duration", duration)) {
    RCLCPP_ERROR(
      rosNode()->get_logger(),
      "ApplyFeedforwardForce: missing required input [duration]");
    return BT::NodeStatus::FAILURE;
  }

  // Normalize axis string to lowercase
  std::transform(
    axis.begin(), axis.end(), axis.begin(), [](unsigned char c) {
      return std::tolower(c);
    });

  if (axis != "x" && axis != "y" && axis != "z") {
    RCLCPP_ERROR(
      rosNode()->get_logger(),
      "ApplyFeedforwardForce: invalid axis '%s'. Must be 'x', 'y', or 'z'.",
      axis.c_str());
    return BT::NodeStatus::FAILURE;
  }

  if (duration <= 0.0) {
    RCLCPP_ERROR(
      rosNode()->get_logger(),
      "ApplyFeedforwardForce: duration must be positive. Got %f",
      duration);
    return BT::NodeStatus::FAILURE;
  }

  duration_ = duration;
  start_time_ = rosNode()->now();

  mercury_msgs::msg::ControllerCommand cmd;
  cmd.mode = mercury_msgs::msg::ControllerCommand::FEEDFORWARD;
  cmd.setpoint_vect.x = (axis == "x") ? force : 0.0;
  cmd.setpoint_vect.y = (axis == "y") ? force : 0.0;
  cmd.setpoint_vect.z = (axis == "z") ? force : 0.0;

  pub_->publish(cmd);

  RCLCPP_INFO(
    rosNode()->get_logger(),
    "Applying feed forward force %f on %s axis for %f seconds",
    force, axis.c_str(), duration);

  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus ApplyFeedforwardForce::onRunning()
{
  double elapsed = (rosNode()->now() - start_time_).seconds();
  if (elapsed >= duration_) {
    // Disable controller to stop applying force
    mercury_msgs::msg::ControllerCommand cmd;
    cmd.mode = mercury_msgs::msg::ControllerCommand::DISABLED;
    pub_->publish(cmd);

    RCLCPP_INFO(
      rosNode()->get_logger(),
      "Finished applying feed forward force (duration %f secs elapsed)",
      duration_);
    return BT::NodeStatus::SUCCESS;
  }

  return BT::NodeStatus::RUNNING;
}

void ApplyFeedforwardForce::onHalted()
{
  // Disable controller if we are halted
  mercury_msgs::msg::ControllerCommand cmd;
  cmd.mode = mercury_msgs::msg::ControllerCommand::DISABLED;
  pub_->publish(cmd);

  RCLCPP_INFO(rosNode()->get_logger(), "Halted applying feed forward force");
}

}  // namespace mercury_autonomy
