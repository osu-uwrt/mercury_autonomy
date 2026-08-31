#include "mercury_autonomy/bt_actions/set_feedforward_mode.hpp"

namespace mercury_autonomy {

namespace {
constexpr char kLinearTopic[] = "controller/linear";
constexpr char kAngularTopic[] = "controller/angular";
}  // namespace

void SetFeedforwardMode::rosInit() {
    linear_pub_ =
        rosNode()->create_publisher<mercury_msgs::msg::ControllerCommand>(kLinearTopic, 10);
    angular_pub_ =
        rosNode()->create_publisher<mercury_msgs::msg::ControllerCommand>(kAngularTopic, 10);
    set_teleop_client_ = rosNode()->create_client<SetBool>("setTeleop");
}

BT::NodeStatus SetFeedforwardMode::onStart() {
    // Disable teleop if the service is active/ready, matching the GUI behavior.
    if (set_teleop_client_->service_is_ready()) {
        auto request = std::make_shared<SetBool::Request>();
        request->data = false;
        set_teleop_client_->async_send_request(request);
        RCLCPP_INFO(rosNode()->get_logger(), "SetFeedforwardMode: Sent setTeleop(false) request");
    } else {
        RCLCPP_DEBUG(
            rosNode()->get_logger(),
            "SetFeedforwardMode: setTeleop service not ready, skipping call");
    }

    // Publish FEEDFORWARD mode command to linear and angular controllers.
    mercury_msgs::msg::ControllerCommand cmd;
    cmd.mode = mercury_msgs::msg::ControllerCommand::FEEDFORWARD;

    // Set default setpoints (zeros/identity)
    cmd.setpoint_vect.x = 0.0;
    cmd.setpoint_vect.y = 0.0;
    cmd.setpoint_vect.z = 0.0;
    cmd.setpoint_quat.x = 0.0;
    cmd.setpoint_quat.y = 0.0;
    cmd.setpoint_quat.z = 0.0;
    cmd.setpoint_quat.w = 1.0;

    linear_pub_->publish(cmd);
    angular_pub_->publish(cmd);

    RCLCPP_INFO(
        rosNode()->get_logger(),
        "SetFeedforwardMode: Published FEEDFORWARD mode to controller/linear and "
        "controller/angular");

    return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus SetFeedforwardMode::onRunning() { return BT::NodeStatus::SUCCESS; }

void SetFeedforwardMode::onHalted() {}

}  // namespace mercury_autonomy
