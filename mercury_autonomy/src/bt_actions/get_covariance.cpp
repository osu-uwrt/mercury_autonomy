// GetCovariance -- computes a single scalar from pose covariance entries.

#include "mercury_autonomy/bt_actions/get_covariance.hpp"

#include <cmath>

namespace mercury_autonomy
{

void GetCovariance::rosInit()
{
}

BT::NodeStatus GetCovariance::onStart()
{
  std::string target;
  getInput("Target", target);
  // Mapping publishes each target pose on a dedicated topic.
  topic_name_ = "mapping/" + target;

  subscriber_ = rosNode()->create_subscription<geometry_msgs::msg::PoseWithCovarianceStamped>(
    topic_name_, rclcpp::SensorDataQoS(),
    [this](geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr msg) {
      topicCallback(msg);
    });

  msg_received_ = false;
  covariance_ = 0.0;
  start_time_ = rosNode()->now();
  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus GetCovariance::onRunning()
{
  if (msg_received_) {
    setOutput("Covariance", covariance_);
    return BT::NodeStatus::SUCCESS;
  }

  if ((rosNode()->now() - start_time_).seconds() > 5.0) {
    RCLCPP_ERROR(
      rosNode()->get_logger(), "GetCovariance: timed out waiting on %s", topic_name_.c_str());
    return BT::NodeStatus::FAILURE;
  }

  return BT::NodeStatus::RUNNING;
}

void GetCovariance::onHalted()
{
}

void GetCovariance::topicCallback(
  const geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr msg)
{
  constexpr double kTwoPi = 6.28318530717958647692;
  covariance_ = 0.0;

  // Use translation and rotation diagonal covariance terms as a compact confidence metric.
  for (int i = 0; i < 3; ++i) {
    covariance_ += std::pow(msg->pose.covariance.at((6 * i) + i), 2);
  }
  for (int i = 3; i < 6; ++i) {
    covariance_ += std::pow(msg->pose.covariance.at((6 * i) + i), 2) / kTwoPi;
  }

  covariance_ = std::sqrt(covariance_);
  msg_received_ = true;
}

}  // namespace mercury_autonomy
