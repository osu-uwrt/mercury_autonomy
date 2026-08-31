// GetOdometry -- subscribes to an Odometry topic and outputs pose components.

#include "mercury_autonomy/bt_actions/get_odometry.hpp"

namespace mercury_autonomy {

void GetOdometry::rosInit() {
    // Subscription is created in onStart so the topic name can be read from ports.
}

BT::NodeStatus GetOdometry::onStart() {
    auto topic = getInput<std::string>("topic");
    if (!topic) {
        RCLCPP_ERROR(rosNode()->get_logger(), "GetOdometry: missing required input [topic]");
        return BT::NodeStatus::FAILURE;
    }

    getInput("timeout", timeout_sec_);
    msg_received_ = false;
    start_time_ = rosNode()->now();

    sub_ = rosNode()->create_subscription<nav_msgs::msg::Odometry>(
        topic.value(), rclcpp::SensorDataQoS(), [this](nav_msgs::msg::Odometry::SharedPtr msg) {
            odom_ = *msg;
            msg_received_ = true;
        });

    RCLCPP_DEBUG(rosNode()->get_logger(), "GetOdometry: subscribing to %s", topic.value().c_str());
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus GetOdometry::onRunning() {
    if (msg_received_) {
        setOutput("x", odom_.pose.pose.position.x);
        setOutput("y", odom_.pose.pose.position.y);
        setOutput("z", odom_.pose.pose.position.z);

        geometry_msgs::msg::Vector3 rpy = toRPY(odom_.pose.pose.orientation);
        setOutput("roll", rpy.x);
        setOutput("pitch", rpy.y);
        setOutput("yaw", rpy.z);

        sub_.reset();
        return BT::NodeStatus::SUCCESS;
    }

    double elapsed = (rosNode()->now() - start_time_).seconds();
    if (elapsed > timeout_sec_) {
        RCLCPP_WARN(rosNode()->get_logger(), "GetOdometry: timed out after %.1f s", timeout_sec_);
        sub_.reset();
        return BT::NodeStatus::FAILURE;
    }

    return BT::NodeStatus::RUNNING;
}

void GetOdometry::onHalted() {
    sub_.reset();
    msg_received_ = false;
}

}  // namespace mercury_autonomy
