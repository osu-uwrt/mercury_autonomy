// WaitForKillSwitch -- subscribes to state/kill and waits until the robot is enabled (kill switch cleared).

#include "mercury_autonomy/bt_actions/wait_for_kill_switch.hpp"

namespace mercury_autonomy {

void WaitForKillSwitch::rosInit() {
    // Subscription is created dynamically in onStart to allow parameter override via ports.
}

BT::NodeStatus WaitForKillSwitch::onStart() {
    std::string topic = "state/kill";
    getInput("topic", topic);

    has_status_ = false;
    is_killed_ = true;

    sub_ = rosNode()->create_subscription<std_msgs::msg::Bool>(
        topic, rclcpp::SensorDataQoS(), [this](std_msgs::msg::Bool::SharedPtr msg) {
            is_killed_ = msg->data;
            has_status_ = true;
        });

    RCLCPP_INFO(rosNode()->get_logger(), "WaitForKillSwitch: subscribed to %s", topic.c_str());
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus WaitForKillSwitch::onRunning() {
    if (has_status_ && !is_killed_) {
        RCLCPP_INFO(
            rosNode()->get_logger(),
            "WaitForKillSwitch: Robot is enabled (kill switch cleared). SUCCESS.");
        sub_.reset();
        return BT::NodeStatus::SUCCESS;
    }

    return BT::NodeStatus::RUNNING;
}

void WaitForKillSwitch::onHalted() {
    sub_.reset();
    has_status_ = false;
}

}  // namespace mercury_autonomy
