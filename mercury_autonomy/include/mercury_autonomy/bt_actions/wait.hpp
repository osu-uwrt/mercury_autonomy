// Wait -- blocks until the requested number of seconds elapse.
//
// Ports:
//   Input: seconds -- Duration to wait before returning SUCCESS.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy
{

class Wait : public MercuryActionNode
{
public:
  Wait(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {BT::InputPort<double>("seconds", 0.0, "Duration to wait")};
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  rclcpp::Time start_time_;
  double goal_time_ = 0.0;
};

}  // namespace mercury_autonomy
