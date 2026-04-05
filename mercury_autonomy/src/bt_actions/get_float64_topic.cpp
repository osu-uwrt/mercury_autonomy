// GetFloat64Topic -- subscribes to a Float64 topic and returns the value once received.

#include "mercury_autonomy/bt_actions/get_float64_topic.hpp"

namespace mercury_autonomy
{

void GetFloat64Topic::rosInit()
{
}

BT::NodeStatus GetFloat64Topic::onStart()
{
  auto topic = getInput<std::string>("topic");
  if (!topic) {
    RCLCPP_ERROR(rosNode()->get_logger(), "GetFloat64Topic: missing required input [topic]");
    return BT::NodeStatus::FAILURE;
  }

  getInput("timeout", timeout_sec_);
  data_received_ = false;
  received_value_ = 0.0;
  start_time_ = rosNode()->now();

  sub_ = rosNode()->create_subscription<std_msgs::msg::Float64>(
    topic.value(), rclcpp::SensorDataQoS(),
    [this](std_msgs::msg::Float64::SharedPtr msg) {
      received_value_ = msg->data;
      data_received_ = true;
    });

  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus GetFloat64Topic::onRunning()
{
  if (data_received_) {
    setOutput("value", received_value_);
    sub_.reset();
    return BT::NodeStatus::SUCCESS;
  }

  if ((rosNode()->now() - start_time_).seconds() > timeout_sec_) {
    RCLCPP_WARN(rosNode()->get_logger(), "GetFloat64Topic: timed out after %.1f s", timeout_sec_);
    sub_.reset();
    return BT::NodeStatus::FAILURE;
  }

  return BT::NodeStatus::RUNNING;
}

void GetFloat64Topic::onHalted()
{
  sub_.reset();
  data_received_ = false;
}

}  // namespace mercury_autonomy
