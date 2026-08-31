// GetOdometry -- subscribes to a nav_msgs/Odometry topic and outputs pose components.
//
// Ports:
//   Input:  topic   -- Odometry topic name.
//   Input:  timeout -- Timeout in seconds (default 3.0).
//   Output: x, y, z -- Position components.
//   Output: roll, pitch, yaw -- Orientation components (radians).

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class GetOdometry : public MercuryActionNode {
public:
    GetOdometry(const std::string & name, const BT::NodeConfig & config)
        : MercuryActionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("topic", "Odometry topic name"),
            BT::InputPort<double>("timeout", 3.0, "Timeout in seconds"),
            BT::OutputPort<double>("x", "Position X"),
            BT::OutputPort<double>("y", "Position Y"),
            BT::OutputPort<double>("z", "Position Z"),
            BT::OutputPort<double>("roll", "Orientation roll (rad)"),
            BT::OutputPort<double>("pitch", "Orientation pitch (rad)"),
            BT::OutputPort<double>("yaw", "Orientation yaw (rad)"),
        };
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

protected:
    void rosInit() override;

private:
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_;
    bool msg_received_ = false;
    nav_msgs::msg::Odometry odom_;
    rclcpp::Time start_time_;
    double timeout_sec_ = 3.0;
};

}  // namespace mercury_autonomy
