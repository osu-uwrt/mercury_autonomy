// PublishInt8 -- publishes a legacy integer command topic.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

#include <std_msgs/msg/u_int16.hpp>

namespace mercury_autonomy
{

class PublishInt8 : public MercuryActionNode
{
public:
  PublishInt8(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<std::string>("topic", "ROS2 topic name to publish to"),
      BT::InputPort<int>("data", 0, "Integer value to publish"),
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  rclcpp::Publisher<std_msgs::msg::UInt16>::SharedPtr pub_;
  std::string last_topic_;
};

}  // namespace mercury_autonomy
