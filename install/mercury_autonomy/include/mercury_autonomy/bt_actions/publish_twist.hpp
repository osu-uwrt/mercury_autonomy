// PublishTwist -- publishes a geometry_msgs/Twist message to a topic.
//
// Ports:
//   Input:  topic      -- The topic name to publish to.
//   Input:  linear_x   -- Linear velocity X component (default 0.0).
//   Input:  linear_y   -- Linear velocity Y component (default 0.0).
//   Input:  linear_z   -- Linear velocity Z component (default 0.0).
//   Input:  angular_x  -- Angular velocity X component (default 0.0).
//   Input:  angular_y  -- Angular velocity Y component (default 0.0).
//   Input:  angular_z  -- Angular velocity Z component (default 0.0).

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy
{

class PublishTwist : public MercuryActionNode
{
public:
  PublishTwist(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<std::string>("topic", "ROS2 topic name to publish to"),
      BT::InputPort<double>("linear_x", 0.0, "Linear velocity X"),
      BT::InputPort<double>("linear_y", 0.0, "Linear velocity Y"),
      BT::InputPort<double>("linear_z", 0.0, "Linear velocity Z"),
      BT::InputPort<double>("angular_x", 0.0, "Angular velocity X"),
      BT::InputPort<double>("angular_y", 0.0, "Angular velocity Y"),
      BT::InputPort<double>("angular_z", 0.0, "Angular velocity Z"),
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  // Publisher is created lazily in onStart so the topic name can come from ports.
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
  std::string last_topic_;
};

}  // namespace mercury_autonomy
