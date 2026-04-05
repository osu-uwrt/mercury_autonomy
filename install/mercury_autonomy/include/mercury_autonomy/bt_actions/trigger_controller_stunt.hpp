// TriggerControllerStunt -- requests a stunt state and waits for confirmation.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

#include <std_msgs/msg/u_int16.hpp>

namespace mercury_autonomy
{

class TriggerControllerStunt : public MercuryActionNode
{
public:
  TriggerControllerStunt(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {BT::InputPort<int>("targetStuntState", 0, "Target stunt state")};
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  void stuntStateCallback(const std_msgs::msg::UInt16::SharedPtr msg);

  rclcpp::Subscription<std_msgs::msg::UInt16>::SharedPtr stunt_state_sub_;
  rclcpp::Publisher<std_msgs::msg::UInt16>::SharedPtr stunt_state_pub_;
  uint16_t stunt_state_ = 0;
  rclcpp::Time start_time_;
  int retry_count_ = 0;
  int target_state_ = 0;
};

}  // namespace mercury_autonomy
