// GetFloatTopic -- subscribes to a std_msgs/Float64 topic and outputs the value.
//
// Ports:
//   Input:  topic     -- The topic name to subscribe to.
//   Input:  timeout   -- Timeout in seconds (default 3.0).
//   Output: value     -- The received floating-point value.

#pragma once

#include <std_msgs/msg/float64.hpp>

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class GetFloatTopic : public MercuryActionNode {
public:
    GetFloatTopic(const std::string & name, const BT::NodeConfig & config)
        : MercuryActionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("topic", "ROS2 topic name to subscribe to"),
            BT::InputPort<double>("timeout", 3.0, "Timeout in seconds"),
            BT::OutputPort<double>("value", "Received floating-point value"),
        };
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

protected:
    void rosInit() override;

private:
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr sub_;
    bool data_received_ = false;
    double received_value_ = 0.0;
    rclcpp::Time start_time_;
    double timeout_sec_ = 3.0;
};

} // namespace mercury_autonomy
