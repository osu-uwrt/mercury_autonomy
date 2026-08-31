// ComputeFrameAlignment -- chains TF transforms to derive a controller alignment pose.

#include "mercury_autonomy/bt_actions/compute_frame_alignment.hpp"

namespace mercury_autonomy {

void ComputeFrameAlignment::rosInit() {}

BT::NodeStatus ComputeFrameAlignment::onStart() {
    getInput("reference_frame", reference_frame_);
    getInput("link_frame", link_frame_);
    getInput("base_frame", base_frame_);

    getInput("x", input_pose_.transform.translation.x);
    getInput("y", input_pose_.transform.translation.y);
    getInput("z", input_pose_.transform.translation.z);

    geometry_msgs::msg::Vector3 rpy;
    getInput("or", rpy.x);
    getInput("op", rpy.y);
    getInput("oy", rpy.z);
    input_pose_.transform.rotation = toQuat(rpy);

    have_ttb_ = false;
    have_twr_ = false;
    start_time_ = rosNode()->now();
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus ComputeFrameAlignment::onRunning() {
    // Resolve both transforms before computing the final chained pose.
    if (!have_ttb_) {
        have_ttb_ = lookupTransform(base_frame_, link_frame_, geom_ttb_);
    }

    if (!have_twr_) {
        have_twr_ = lookupTransform(reference_frame_, "world", geom_twr_);
    }

    if (have_ttb_ && have_twr_) {
        // Compose world<-reference<-input<-base to align controller commands to world.
        tf2::Transform tf_ttb;
        tf2::fromMsg(geom_ttb_.transform, tf_ttb);

        tf2::Transform tf_twr;
        tf2::fromMsg(geom_twr_.transform, tf_twr);

        tf2::Transform tf_trt;
        tf2::fromMsg(input_pose_.transform, tf_trt);

        const tf2::Transform tf_twb = tf_twr * tf_trt * tf_ttb;

        geometry_msgs::msg::TransformStamped out;
        out.transform = tf2::toMsg(tf_twb);
        const auto out_rpy = toRPY(out.transform.rotation);
        setOutput("out_x", out.transform.translation.x);
        setOutput("out_y", out.transform.translation.y);
        setOutput("out_z", out.transform.translation.z);
        setOutput("out_or", out_rpy.x);
        setOutput("out_op", out_rpy.y);
        setOutput("out_oy", out_rpy.z);
        return BT::NodeStatus::SUCCESS;
    }

    if ((rosNode()->now() - start_time_).seconds() < 5.0) {
        return BT::NodeStatus::RUNNING;
    }

    RCLCPP_ERROR(rosNode()->get_logger(), "ComputeFrameAlignment: failed to calculate alignment");
    return BT::NodeStatus::FAILURE;
}

void ComputeFrameAlignment::onHalted() {}

} // namespace mercury_autonomy
