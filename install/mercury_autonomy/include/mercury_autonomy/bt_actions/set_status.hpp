// SetStatus -- publishes a legacy LED status command.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

#include <mercury_msgs/msg/led_command.hpp>

namespace mercury_autonomy
{

class SetStatus : public MercuryActionNode
{
public:
  SetStatus(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {BT::InputPort<std::string>("status", "Behavior status name")};
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  rclcpp::Publisher<mercury_msgs::msg::LedCommand>::SharedPtr pub_;
};

}  // namespace mercury_autonomy
