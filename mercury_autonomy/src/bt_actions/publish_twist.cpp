// PublishTwist -- publishes a Twist message and returns SUCCESS immediately.

#include "mercury_autonomy/bt_actions/publish_twist.hpp"

namespace mercury_autonomy
{

void PublishTwist::rosInit()
{
  // Publisher is created in onStart so the topic name can be read from ports.
}

BT::NodeStatus PublishTwist::onStart()
{
  auto topic = getInput<std::string>("topic");
  if (!topic) {
    RCLCPP_ERROR(rosNode()->get_logger(), "PublishTwist: missing required input [topic]");
    return BT::NodeStatus::FAILURE;
  }

  // Re-create publisher only when the topic name changes
  if (!pub_ || topic.value() != last_topic_) {
    pub_ = rosNode()->create_publisher<geometry_msgs::msg::Twist>(topic.value(), 10);
    last_topic_ = topic.value();
  }

  geometry_msgs::msg::Twist msg;
  getInput("linear_x", msg.linear.x);
  getInput("linear_y", msg.linear.y);
  getInput("linear_z", msg.linear.z);
  getInput("angular_x", msg.angular.x);
  getInput("angular_y", msg.angular.y);
  getInput("angular_z", msg.angular.z);

  pub_->publish(msg);
  RCLCPP_DEBUG(
    rosNode()->get_logger(), "PublishTwist: published to %s", topic.value().c_str());

  return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus PublishTwist::onRunning()
{
  // Single-shot action -- should not reach RUNNING state
  return BT::NodeStatus::SUCCESS;
}

void PublishTwist::onHalted()
{
  // Nothing to clean up for a single-shot publish
}

}  // namespace mercury_autonomy
