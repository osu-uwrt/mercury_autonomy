// GetImuOrientation -- subscribes to a sensor_msgs/Imu topic and outputs RPY.
//
// Ports:
//   Input:  topic   -- IMU topic name.
//   Input:  timeout -- Timeout in seconds (default 3.0).
//   Output: roll, pitch, yaw -- Orientation in radians (converted from quaternion).

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy
{

class GetImuOrientation : public MercuryActionNode
{
public:
  GetImuOrientation(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<std::string>("topic", "IMU topic name"),
      BT::InputPort<double>("timeout", 3.0, "Timeout in seconds"),
      BT::OutputPort<double>("roll", "Orientation roll (rad)"),
      BT::OutputPort<double>("pitch", "Orientation pitch (rad)"),
      BT::OutputPort<double>("yaw", "Orientation yaw (rad)"),
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr sub_;
  bool msg_received_ = false;
  geometry_msgs::msg::Vector3 last_rpy_;
  rclcpp::Time start_time_;
  double timeout_sec_ = 3.0;
};

}  // namespace mercury_autonomy
