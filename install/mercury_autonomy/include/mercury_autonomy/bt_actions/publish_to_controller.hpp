// PublishToController -- publishes controller commands to the linear or angular topic.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

#include <mercury_msgs/msg/controller_command.hpp>

namespace mercury_autonomy
{

class PublishToController : public MercuryActionNode
{
public:
  PublishToController(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<bool>("isOrientation", false, "Publish orientation command"),
      BT::InputPort<int>("mode", 3, "Controller mode"),
      BT::InputPort<double>("x", 0.0, "X command value"),
      BT::InputPort<double>("y", 0.0, "Y command value"),
      BT::InputPort<double>("z", 0.0, "Z command value"),
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  rclcpp::Publisher<mercury_msgs::msg::ControllerCommand>::SharedPtr position_pub_;
  rclcpp::Publisher<mercury_msgs::msg::ControllerCommand>::SharedPtr orientation_pub_;
};

}  // namespace mercury_autonomy
