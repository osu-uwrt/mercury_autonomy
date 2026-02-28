// PublishBool -- publishes a Bool message and returns SUCCESS immediately.

#include "mercury_autonomy/bt_actions/publish_bool.hpp"

namespace mercury_autonomy
{

void PublishBool::rosInit()
{
  // Publisher is created in onStart so the topic name can be read from ports.
}

BT::NodeStatus PublishBool::onStart()
{
  auto topic = getInput<std::string>("topic");
  if (!topic) {
    RCLCPP_ERROR(rosNode()->get_logger(), "PublishBool: missing required input [topic]");
    return BT::NodeStatus::FAILURE;
  }

  // Re-create publisher only when the topic name changes
  if (!pub_ || topic.value() != last_topic_) {
    pub_ = rosNode()->create_publisher<std_msgs::msg::Bool>(topic.value(), 10);
    last_topic_ = topic.value();
  }

  bool val = true;
  getInput("value", val);

  std_msgs::msg::Bool msg;
  msg.data = val;
  pub_->publish(msg);

  RCLCPP_DEBUG(
    rosNode()->get_logger(), "PublishBool: published %s to %s",
    val ? "true" : "false", topic.value().c_str());

  return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus PublishBool::onRunning()
{
  // Single-shot action -- should not reach RUNNING state
  return BT::NodeStatus::SUCCESS;
}

void PublishBool::onHalted()
{
  // Nothing to clean up for a single-shot publish
}

}  // namespace mercury_autonomy
