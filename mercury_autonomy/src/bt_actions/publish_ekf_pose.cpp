// PublishEKFPose -- pushes a pose update into robot_localization.

#include "mercury_autonomy/bt_actions/publish_ekf_pose.hpp"

namespace mercury_autonomy {

namespace {
constexpr char kOdomTopic[] = "odometry/filtered";
constexpr char kSetPoseService[] = "set_pose";
}  // namespace

void PublishEKFPose::rosInit() {
    pose_client_ = rosNode()->create_client<robot_localization::srv::SetPose>(kSetPoseService);
    odom_sub_ = rosNode()->create_subscription<nav_msgs::msg::Odometry>(
        kOdomTopic, 10, [this](nav_msgs::msg::Odometry::SharedPtr msg) { odomCallback(msg); });
}

BT::NodeStatus PublishEKFPose::onStart() {
    odom_received_ = false;

    if (!pose_client_->wait_for_service(std::chrono::seconds(1))) {
        RCLCPP_ERROR(
            rosNode()->get_logger(), "PublishEKFPose: service %s is not available",
            kSetPoseService);
        return BT::NodeStatus::FAILURE;
    }

    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus PublishEKFPose::onRunning() {
    if (!odom_received_) {
        // Keep the action running until we have a baseline pose to merge with overrides.
        return BT::NodeStatus::RUNNING;
    }

    auto request = std::make_shared<robot_localization::srv::SetPose::Request>();
    request->pose.header.stamp = rosNode()->now();
    request->pose.header.frame_id = "odom";
    request->pose.pose.covariance.fill(0.0);

    bool set_orientation = false;
    getInput("setOrientation", set_orientation);
    if (set_orientation) {
        double roll = 0.0;
        double pitch = 0.0;
        double yaw = 0.0;
        getInput("roll", roll);
        getInput("pitch", pitch);
        getInput("yaw", yaw);

        geometry_msgs::msg::Vector3 rpy;
        rpy.x = roll;
        rpy.y = pitch;
        rpy.z = yaw;
        request->pose.pose.pose.orientation = toQuat(rpy);
    } else {
        // Preserve the current EKF orientation when no override was requested.
        request->pose.pose.pose.orientation = odom_msg_.pose.pose.orientation;
    }

    bool set_x = false;
    bool set_y = false;
    bool set_z = false;
    getInput("setX", set_x);
    getInput("setY", set_y);
    getInput("setZ", set_z);

    if (set_x) {
        getInput("x", request->pose.pose.pose.position.x);
    } else {
        request->pose.pose.pose.position.x = odom_msg_.pose.pose.position.x;
    }
    if (set_y) {
        getInput("y", request->pose.pose.pose.position.y);
    } else {
        request->pose.pose.pose.position.y = odom_msg_.pose.pose.position.y;
    }
    if (set_z) {
        getInput("z", request->pose.pose.pose.position.z);
    } else {
        request->pose.pose.pose.position.z = odom_msg_.pose.pose.position.z;
    }

    pose_client_->async_send_request(request);
    RCLCPP_INFO(rosNode()->get_logger(), "PublishEKFPose: pose update sent");
    return BT::NodeStatus::SUCCESS;
}

void PublishEKFPose::onHalted() {}

void PublishEKFPose::odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg) {
    odom_msg_ = *msg;
    odom_received_ = true;
}

}  // namespace mercury_autonomy
