// WaitForKillSwitch -- subscribes to state/kill and waits until the robot is enabled (kill switch
// cleared).
//
// Ports:
//   Input:  topic     -- The topic name to subscribe to (default "state/kill").

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class WaitForKillSwitch : public MercuryActionNode {
public:
    WaitForKillSwitch(const std::string & name, const BT::NodeConfig & config)
        : MercuryActionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>(
                "topic", "state/kill", "ROS2 topic name for the kill switch state"),
        };
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

protected:
    void rosInit() override;

private:
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr sub_;
    bool has_status_ = false;
    bool is_killed_ = true;
};

} // namespace mercury_autonomy
