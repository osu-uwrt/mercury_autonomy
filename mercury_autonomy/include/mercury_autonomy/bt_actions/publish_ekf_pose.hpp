// PublishEKFPose -- updates robot_localization pose from the current odometry.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

#include <robot_localization/srv/set_pose.hpp>

namespace mercury_autonomy
{

class PublishEKFPose : public MercuryActionNode
{
public:
  PublishEKFPose(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<double>("x", 0.0, "Position X"),
      BT::InputPort<bool>("setX", false, "Apply X position"),
      BT::InputPort<double>("y", 0.0, "Position Y"),
      BT::InputPort<bool>("setY", false, "Apply Y position"),
      BT::InputPort<double>("z", 0.0, "Position Z"),
      BT::InputPort<bool>("setZ", false, "Apply Z position"),
      BT::InputPort<double>("roll", 0.0, "Orientation roll"),
      BT::InputPort<double>("pitch", 0.0, "Orientation pitch"),
      BT::InputPort<double>("yaw", 0.0, "Orientation yaw"),
      BT::InputPort<bool>("setOrientation", false, "Apply orientation"),
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg);

  rclcpp::Client<robot_localization::srv::SetPose>::SharedPtr pose_client_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
  nav_msgs::msg::Odometry odom_msg_;
  bool odom_received_ = false;
};

}  // namespace mercury_autonomy
