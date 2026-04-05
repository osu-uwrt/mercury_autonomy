// GetMappingState -- subscribes to mapping state and outputs the current target.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

#include <mercury_msgs/msg/mapping_target_info.hpp>

namespace mercury_autonomy
{

class GetMappingState : public MercuryActionNode
{
public:
  GetMappingState(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::OutputPort<bool>("map_locked", "Whether mapping is locked"),
      BT::OutputPort<std::string>("target_name", "Current mapping target"),
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  rclcpp::Subscription<mercury_msgs::msg::MappingTargetInfo>::SharedPtr sub_;
  bool msg_received_ = false;
  mercury_msgs::msg::MappingTargetInfo latest_msg_;
  rclcpp::Time start_time_;
};

}  // namespace mercury_autonomy
