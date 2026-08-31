// GetTwistTopic -- subscribes to a geometry_msgs/Twist topic and outputs components.
//
// Ports:
//   Input:  topic   -- Twist topic name.
//   Input:  timeout -- Timeout in seconds (default 3.0).
//   Output: vel_x, vel_y, vel_z -- Linear velocity components.
//   Output: vel_roll, vel_pitch, vel_yaw -- Angular velocity components.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class GetTwistTopic : public MercuryActionNode {
public:
    GetTwistTopic(const std::string & name, const BT::NodeConfig & config)
        : MercuryActionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("topic", "Twist topic name"),
            BT::InputPort<double>("timeout", 3.0, "Timeout in seconds"),
            BT::OutputPort<double>("vel_x", "Linear velocity X"),
            BT::OutputPort<double>("vel_y", "Linear velocity Y"),
            BT::OutputPort<double>("vel_z", "Linear velocity Z"),
            BT::OutputPort<double>("vel_roll", "Angular velocity X (roll rate)"),
            BT::OutputPort<double>("vel_pitch", "Angular velocity Y (pitch rate)"),
            BT::OutputPort<double>("vel_yaw", "Angular velocity Z (yaw rate)"),
        };
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

protected:
    void rosInit() override;

private:
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_;
    bool msg_received_ = false;
    geometry_msgs::msg::Twist twist_;
    rclcpp::Time start_time_;
    double timeout_sec_ = 3.0;
};

}  // namespace mercury_autonomy
