// ApplyFeedforwardForce BT action node implementation.
// Continously publishes forces in all 6 axes for a certain duration of seconds.

#pragma once

#include <geometry_msgs/msg/twist.hpp>

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class ApplyFeedforwardForce : public MercuryActionNode {
public:
    ApplyFeedforwardForce(const std::string & name, const BT::NodeConfig & config)
        : MercuryActionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<double>("linear_x", 0.0, "X linear force to apply"),
            BT::InputPort<double>("linear_y", 0.0, "Y linear force to apply"),
            BT::InputPort<double>("linear_z", 0.0, "Z linear force to apply"),
            BT::InputPort<double>("angular_x", 0.0, "X angular torque to apply"),
            BT::InputPort<double>("angular_y", 0.0, "Y angular torque to apply"),
            BT::InputPort<double>("angular_z", 0.0, "Z angular torque to apply"),
            BT::InputPort<double>("duration", "Time duration to apply force in seconds"),
        };
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

protected:
    void rosInit() override;

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    geometry_msgs::msg::Twist cmd_msg_;
    rclcpp::Time start_time_;
    double duration_;
};

}  // namespace mercury_autonomy
