#include "mercury_autonomy/bt_actions/apply_feedforward_force.hpp"
#include <algorithm>
#include <cctype>

namespace mercury_autonomy
{

void ApplyFeedforwardForce::rosInit()
{
  pub_ = rosNode()->create_publisher<geometry_msgs::msg::Twist>(
    "controller/FF_body_force", 10);
  sub_ = rosNode()->create_subscription<geometry_msgs::msg::Twist>(
    "controller/FF_body_force", 10,
    [this](geometry_msgs::msg::Twist::SharedPtr msg) {
      last_msg_ = *msg;
      has_last_msg_ = true;
    });
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

  if (has_last_msg_) {
    baseline_msg_ = last_msg_;
  } else {
    baseline_msg_ = geometry_msgs::msg::Twist();
  }

  geometry_msgs::msg::Twist cmd = baseline_msg_;
  if (axis == "x") {
    cmd.linear.x += force;
  } else if (axis == "y") {
    cmd.linear.y += force;
  } else if (axis == "z") {
    cmd.linear.z += force;
  }

  pub_->publish(cmd);

  RCLCPP_INFO(
    rosNode()->get_logger(),
    "Applying feed forward force %f on %s axis to controller/FF_body_force for %f seconds",
    force, axis.c_str(), duration);

  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus ApplyFeedforwardForce::onRunning()
{
  double elapsed = (rosNode()->now() - start_time_).seconds();
  if (elapsed >= duration_) {
    pub_->publish(baseline_msg_);

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
  pub_->publish(baseline_msg_);

  RCLCPP_INFO(rosNode()->get_logger(), "Halted applying feed forward force");
}

}  // namespace mercury_autonomy
