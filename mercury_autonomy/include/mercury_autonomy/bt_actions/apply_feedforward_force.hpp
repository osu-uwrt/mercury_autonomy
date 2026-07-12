// ApplyFeedforwardForce BT action node implementation.
// Applies a feed forward force for a certain translational axis (x, y, or z)
// for a certain amount of time, then disables the force.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"
#include <mercury_msgs/msg/controller_command.hpp>

namespace mercury_autonomy
{

class ApplyFeedforwardForce : public MercuryActionNode
{
public:
  ApplyFeedforwardForce(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<std::string>("axis", "Translational axis to apply force to (x, y, or z)"),
      BT::InputPort<double>("force", "Feedforward force value to apply"),
      BT::InputPort<double>("duration", "Time duration to apply force in seconds"),
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  rclcpp::Publisher<mercury_msgs::msg::ControllerCommand>::SharedPtr pub_;
  rclcpp::Time start_time_;
  double duration_;
};

}  // namespace mercury_autonomy
