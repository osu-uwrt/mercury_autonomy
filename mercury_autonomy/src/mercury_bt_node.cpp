// MercuryBtNode implementation -- provides ROS2 integration for custom BT nodes.

#include "mercury_autonomy/mercury_bt_node.hpp"

namespace mercury_autonomy {

// Static member definitions
std::shared_ptr<tf2_ros::Buffer> MercuryBtNode::tf_buffer_ = nullptr;
std::shared_ptr<tf2_ros::TransformListener> MercuryBtNode::tf_listener_ = nullptr;

void MercuryBtNode::staticInit(rclcpp::Node::SharedPtr node) {
    if (!tf_buffer_) {
        tf_buffer_ = std::make_shared<tf2_ros::Buffer>(node->get_clock());
    }
    if (!tf_listener_) {
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
    }
}

void MercuryBtNode::staticDeinit() {
    tf_listener_.reset();
    tf_buffer_.reset();
}

void MercuryBtNode::init(rclcpp::Node::SharedPtr node) {
    ros_node_ = node;
    rosInit();
}

bool MercuryBtNode::lookupTransform(
    const std::string & from_frame, const std::string & to_frame,
    geometry_msgs::msg::TransformStamped & transform, bool use_current_time) {
    try {
        auto time_point = use_current_time ? tf2_ros::fromRclcpp(ros_node_->get_clock()->now())
                                           : tf2::TimePointZero;
        transform = tf_buffer_->lookupTransform(to_frame, from_frame, time_point);
        return true;
    } catch (const tf2::TransformException & ex) {
        RCLCPP_WARN_THROTTLE(
            ros_node_->get_logger(), *ros_node_->get_clock(), 1000,
            "TF lookup failed (%s -> %s): %s", from_frame.c_str(), to_frame.c_str(), ex.what());
    }
    return false;
}

}  // namespace mercury_autonomy
