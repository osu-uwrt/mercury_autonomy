// GetBoolTopic -- subscribes to a std_msgs/Bool topic and outputs the value.
//
// Ports:
//   Input:  topic     -- The topic name to subscribe to.
//   Input:  timeout   -- Timeout in seconds (default 3.0).
//   Output: value     -- The received boolean value.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy
{

class GetBoolTopic : public MercuryActionNode
{
public:
  GetBoolTopic(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<std::string>("topic", "ROS2 topic name to subscribe to"),
      BT::InputPort<double>("timeout", 3.0, "Timeout in seconds"),
      BT::OutputPort<bool>("value", "Received boolean value"),
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr sub_;
  bool data_received_ = false;
  bool received_value_ = false;
  rclcpp::Time start_time_;
  double timeout_sec_ = 3.0;
};

}  // namespace mercury_autonomy
