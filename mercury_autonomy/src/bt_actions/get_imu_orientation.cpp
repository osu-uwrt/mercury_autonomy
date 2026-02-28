// GetImuOrientation -- subscribes to an IMU topic, outputs orientation as RPY.

#include "mercury_autonomy/bt_actions/get_imu_orientation.hpp"

namespace mercury_autonomy
{

void GetImuOrientation::rosInit()
{
  // Subscription is created in onStart so the topic name can be read from ports.
}

BT::NodeStatus GetImuOrientation::onStart()
{
  auto topic = getInput<std::string>("topic");
  if (!topic) {
    RCLCPP_ERROR(rosNode()->get_logger(), "GetImuOrientation: missing required input [topic]");
    return BT::NodeStatus::FAILURE;
  }

  getInput("timeout", timeout_sec_);
  msg_received_ = false;
  start_time_ = rosNode()->now();

  sub_ = rosNode()->create_subscription<sensor_msgs::msg::Imu>(
    topic.value(), rclcpp::SensorDataQoS(),
    [this](sensor_msgs::msg::Imu::SharedPtr msg) {
      last_rpy_ = toRPY(msg->orientation);
      msg_received_ = true;
    });

  RCLCPP_DEBUG(
    rosNode()->get_logger(), "GetImuOrientation: subscribing to %s",
    topic.value().c_str());
  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus GetImuOrientation::onRunning()
{
  if (msg_received_) {
    setOutput("roll", last_rpy_.x);
    setOutput("pitch", last_rpy_.y);
    setOutput("yaw", last_rpy_.z);
    sub_.reset();
    return BT::NodeStatus::SUCCESS;
  }

  double elapsed = (rosNode()->now() - start_time_).seconds();
  if (elapsed > timeout_sec_) {
    RCLCPP_WARN(
      rosNode()->get_logger(), "GetImuOrientation: timed out after %.1f s", timeout_sec_);
    sub_.reset();
    return BT::NodeStatus::FAILURE;
  }

  return BT::NodeStatus::RUNNING;
}

void GetImuOrientation::onHalted()
{
  sub_.reset();
  msg_received_ = false;
}

}  // namespace mercury_autonomy
