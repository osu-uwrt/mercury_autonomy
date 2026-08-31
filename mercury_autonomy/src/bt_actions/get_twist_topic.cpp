// GetTwistTopic -- subscribes to a Twist topic, outputs velocity components.

#include "mercury_autonomy/bt_actions/get_twist_topic.hpp"

namespace mercury_autonomy {

void GetTwistTopic::rosInit() {
    // Subscription is created in onStart so the topic name can be read from ports.
}

BT::NodeStatus GetTwistTopic::onStart() {
    auto topic = getInput<std::string>("topic");
    if (!topic) {
        RCLCPP_ERROR(rosNode()->get_logger(), "GetTwistTopic: missing required input [topic]");
        return BT::NodeStatus::FAILURE;
    }

    getInput("timeout", timeout_sec_);
    msg_received_ = false;
    start_time_ = rosNode()->now();

    sub_ = rosNode()->create_subscription<geometry_msgs::msg::Twist>(
        topic.value(), rclcpp::SensorDataQoS(), [this](geometry_msgs::msg::Twist::SharedPtr msg) {
            twist_ = *msg;
            msg_received_ = true;
        });

    RCLCPP_DEBUG(
        rosNode()->get_logger(), "GetTwistTopic: subscribing to %s", topic.value().c_str());
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus GetTwistTopic::onRunning() {
    if (msg_received_) {
        setOutput("vel_x", twist_.linear.x);
        setOutput("vel_y", twist_.linear.y);
        setOutput("vel_z", twist_.linear.z);
        setOutput("vel_roll", twist_.angular.x);
        setOutput("vel_pitch", twist_.angular.y);
        setOutput("vel_yaw", twist_.angular.z);
        sub_.reset();
        return BT::NodeStatus::SUCCESS;
    }

    double elapsed = (rosNode()->now() - start_time_).seconds();
    if (elapsed > timeout_sec_) {
        RCLCPP_WARN(rosNode()->get_logger(), "GetTwistTopic: timed out after %.1f s", timeout_sec_);
        sub_.reset();
        return BT::NodeStatus::FAILURE;
    }

    return BT::NodeStatus::RUNNING;
}

void GetTwistTopic::onHalted() {
    sub_.reset();
    msg_received_ = false;
}

} // namespace mercury_autonomy
