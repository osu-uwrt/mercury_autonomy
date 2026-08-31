// TransformPose -- looks up and applies a TF2 transform.

#include "mercury_autonomy/bt_actions/transform_pose.hpp"

namespace mercury_autonomy {

void TransformPose::rosInit() {
    // TF buffer is provided by MercuryBtNode; no additional setup needed.
}

BT::NodeStatus TransformPose::onStart() {
    auto from = getInput<std::string>("from_frame");
    auto to = getInput<std::string>("to_frame");
    if (!from || !to) {
        RCLCPP_ERROR(rosNode()->get_logger(), "TransformPose: missing frame inputs");
        return BT::NodeStatus::FAILURE;
    }
    from_frame_ = from.value();
    to_frame_ = to.value();

    // Build the original pose from input ports
    double x = 0, y = 0, z = 0, roll = 0, pitch = 0, yaw = 0;
    getInput("x", x);
    getInput("y", y);
    getInput("z", z);
    getInput("roll", roll);
    getInput("pitch", pitch);
    getInput("yaw", yaw);
    getInput("timeout", timeout_sec_);

    original_.position.x = x;
    original_.position.y = y;
    original_.position.z = z;

    geometry_msgs::msg::Vector3 rpy;
    rpy.x = roll;
    rpy.y = pitch;
    rpy.z = yaw;
    original_.orientation = toQuat(rpy);

    start_time_ = rosNode()->now();
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus TransformPose::onRunning() {
    geometry_msgs::msg::TransformStamped transform;
    bool found = lookupTransform(from_frame_, to_frame_, transform);

    if (found) {
        geometry_msgs::msg::Pose result = doTransform(original_, transform);
        geometry_msgs::msg::Vector3 out_rpy = toRPY(result.orientation);

        setOutput("out_x", result.position.x);
        setOutput("out_y", result.position.y);
        setOutput("out_z", result.position.z);
        setOutput("out_roll", out_rpy.x);
        setOutput("out_pitch", out_rpy.y);
        setOutput("out_yaw", out_rpy.z);

        RCLCPP_DEBUG(
            rosNode()->get_logger(),
            "TransformPose: %s->%s => XYZ(%.3f, %.3f, %.3f) RPY(%.3f, %.3f, %.3f)",
            from_frame_.c_str(), to_frame_.c_str(), result.position.x, result.position.y,
            result.position.z, out_rpy.x, out_rpy.y, out_rpy.z);

        return BT::NodeStatus::SUCCESS;
    }

    // Transform not yet available -- check timeout
    double elapsed = (rosNode()->now() - start_time_).seconds();
    if (elapsed > timeout_sec_) {
        RCLCPP_ERROR(
            rosNode()->get_logger(), "TransformPose: timed out looking up %s -> %s",
            from_frame_.c_str(), to_frame_.c_str());
        return BT::NodeStatus::FAILURE;
    }

    return BT::NodeStatus::RUNNING;
}

void TransformPose::onHalted() {
    // Nothing to clean up.
}

}  // namespace mercury_autonomy
