#include "mercury_autonomy/bt_actions/apply_feedforward_force.hpp"

namespace mercury_autonomy {

void ApplyFeedforwardForce::rosInit() {
    pub_ = rosNode()->create_publisher<geometry_msgs::msg::Twist>("controller/FF_body_force", 10);
}

BT::NodeStatus ApplyFeedforwardForce::onStart() {
    double duration = 0.0;

    if (!getInput("duration", duration)) {
        RCLCPP_ERROR(
            rosNode()->get_logger(), "ApplyFeedforwardForce: missing required input [duration]");
        return BT::NodeStatus::FAILURE;
    }

    if (duration <= 0.0) {
        RCLCPP_ERROR(
            rosNode()->get_logger(), "ApplyFeedforwardForce: duration must be positive. Got %f",
            duration);
        return BT::NodeStatus::FAILURE;
    }

    duration_ = duration;
    start_time_ = rosNode()->now();

    cmd_msg_ = geometry_msgs::msg::Twist();
    getInput("linear_x", cmd_msg_.linear.x);
    getInput("linear_y", cmd_msg_.linear.y);
    getInput("linear_z", cmd_msg_.linear.z);
    getInput("angular_x", cmd_msg_.angular.x);
    getInput("angular_y", cmd_msg_.angular.y);
    getInput("angular_z", cmd_msg_.angular.z);

    pub_->publish(cmd_msg_);

    RCLCPP_INFO(
        rosNode()->get_logger(),
        "Applying feed forward force: linear(%f, %f, %f), angular(%f, %f, %f) for %f seconds",
        cmd_msg_.linear.x, cmd_msg_.linear.y, cmd_msg_.linear.z, cmd_msg_.angular.x,
        cmd_msg_.angular.y, cmd_msg_.angular.z, duration);

    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus ApplyFeedforwardForce::onRunning() {
    double elapsed = (rosNode()->now() - start_time_).seconds();
    if (elapsed >= duration_) {
        pub_->publish(geometry_msgs::msg::Twist());

        RCLCPP_INFO(
            rosNode()->get_logger(),
            "Finished applying feed forward force (duration %f secs elapsed)", duration_);
        return BT::NodeStatus::SUCCESS;
    }

    pub_->publish(cmd_msg_);
    return BT::NodeStatus::RUNNING;
}

void ApplyFeedforwardForce::onHalted() {
    pub_->publish(geometry_msgs::msg::Twist());

    RCLCPP_INFO(rosNode()->get_logger(), "Halted applying feed forward force");
}

}  // namespace mercury_autonomy
