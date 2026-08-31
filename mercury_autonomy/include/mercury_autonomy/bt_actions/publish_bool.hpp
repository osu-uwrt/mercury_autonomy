// PublishBool -- publishes a std_msgs/Bool message to a topic.
//
// Ports:
//   Input:  topic  -- The topic name to publish to.
//   Input:  value  -- The boolean value to publish (default true).

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class PublishBool : public MercuryActionNode {
public:
    PublishBool(const std::string & name, const BT::NodeConfig & config)
        : MercuryActionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("topic", "ROS2 topic name to publish to"),
            BT::InputPort<bool>("value", true, "Boolean value to publish"),
        };
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

protected:
    void rosInit() override;

private:
    // Publisher created lazily in onStart so the topic name can come from ports.
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr pub_;
    std::string last_topic_;
};

} // namespace mercury_autonomy
