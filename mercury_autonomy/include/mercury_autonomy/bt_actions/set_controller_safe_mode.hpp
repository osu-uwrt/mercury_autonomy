// SetControllerSafeMode -- toggles the controller stunt safe mode parameter.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"
#include <rclcpp/parameter_client.hpp>

namespace mercury_autonomy
{

class SetControllerSafeMode : public MercuryActionNode
{
public:
  SetControllerSafeMode(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<bool>("safe", true, "Enable safe mode"),
      BT::InputPort<double>("timeout_secs", 0.0, "Wait timeout in seconds"),
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  void onSetParametersDone(
    std::shared_future<std::vector<rcl_interfaces::msg::SetParametersResult>> future);

  rclcpp::AsyncParametersClient::SharedPtr async_client_;
  bool set_complete_ = false;
  double timeout_secs_ = 0.0;
  rclcpp::Time start_time_;
};

}  // namespace mercury_autonomy
