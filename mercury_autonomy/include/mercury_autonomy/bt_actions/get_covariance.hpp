// getCovariance -- computes a scalar covariance score for a mapping pose.
//
// Ports:
//   Input:  Target      -- Mapping target name.
//   Output: Covariance  -- Scalar uncertainty score from covariance diagonals.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>

namespace mercury_autonomy
{

class GetCovariance : public MercuryActionNode
{
public:
  GetCovariance(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<std::string>("Target", "Mapping target name"),
      BT::OutputPort<double>("Covariance", "Computed covariance score"),
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  // Converts the latest mapping pose covariance matrix into a scalar score.
  void topicCallback(const geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr msg);

  rclcpp::Subscription<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr subscriber_;
  bool msg_received_ = false;
  double covariance_ = 0.0;
  rclcpp::Time start_time_;
  std::string topic_name_;
};

}  // namespace mercury_autonomy
