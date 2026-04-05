// TriggerControllerStunt -- publishes a target stunt state and retries until confirmed.

#include "mercury_autonomy/bt_actions/trigger_controller_stunt.hpp"

namespace mercury_autonomy
{

namespace
{
constexpr char kTriggerTopic[] = "controller/stunt_state";
constexpr char kStatusTopic[] = "controller/running_stunt_state";
constexpr int kMaxRetry = 5;
constexpr double kRetryIntervalSec = 2.0;
}

void TriggerControllerStunt::rosInit()
{
  stunt_state_sub_ = rosNode()->create_subscription<std_msgs::msg::UInt16>(
    kStatusTopic, 10,
    [this](std_msgs::msg::UInt16::SharedPtr msg) {stuntStateCallback(msg);});
  stunt_state_pub_ = rosNode()->create_publisher<std_msgs::msg::UInt16>(kTriggerTopic, 10);
}

BT::NodeStatus TriggerControllerStunt::onStart()
{
  getInput("targetStuntState", target_state_);
  retry_count_ = 0;
  start_time_ = rosNode()->now();

  std_msgs::msg::UInt16 msg;
  msg.data = static_cast<uint16_t>(target_state_);
  stunt_state_pub_->publish(msg);
  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus TriggerControllerStunt::onRunning()
{
  if (stunt_state_ == static_cast<uint16_t>(target_state_)) {
    return BT::NodeStatus::SUCCESS;
  }

  const double elapsed = (rosNode()->now() - start_time_).seconds();
  if (elapsed > static_cast<double>(retry_count_) + kRetryIntervalSec) {
    std_msgs::msg::UInt16 msg;
    msg.data = static_cast<uint16_t>(target_state_);
    stunt_state_pub_->publish(msg);
    ++retry_count_;
  }

  if (retry_count_ > kMaxRetry) {
    return BT::NodeStatus::FAILURE;
  }

  return BT::NodeStatus::RUNNING;
}

void TriggerControllerStunt::onHalted()
{
}

void TriggerControllerStunt::stuntStateCallback(const std_msgs::msg::UInt16::SharedPtr msg)
{
  stunt_state_ = msg->data;
}

}  // namespace mercury_autonomy
