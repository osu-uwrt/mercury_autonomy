// PublishToController -- publishes a controller command immediately.

#include "mercury_autonomy/bt_actions/publish_to_controller.hpp"

namespace mercury_autonomy {

namespace {
constexpr char kLinearTopic[] = "controller/linear";
constexpr char kAngularTopic[] = "controller/angular";
} // namespace

void PublishToController::rosInit() {
    position_pub_ =
        rosNode()->create_publisher<mercury_msgs::msg::ControllerCommand>(kLinearTopic, 10);
    orientation_pub_ =
        rosNode()->create_publisher<mercury_msgs::msg::ControllerCommand>(kAngularTopic, 10);
}

BT::NodeStatus PublishToController::onStart() {
    bool is_orientation = false;
    int mode = mercury_msgs::msg::ControllerCommand::POSITION;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    getInput("isOrientation", is_orientation);
    getInput("mode", mode);
    getInput("x", x);
    getInput("y", y);
    getInput("z", z);

    mercury_msgs::msg::ControllerCommand cmd;
    cmd.mode = static_cast<uint8_t>(mode);
    cmd.setpoint_vect.x = x;
    cmd.setpoint_vect.y = y;
    cmd.setpoint_vect.z = z;

    if (is_orientation && cmd.mode == mercury_msgs::msg::ControllerCommand::POSITION) {
        cmd.setpoint_quat = toQuat(cmd.setpoint_vect);
    }

    if (is_orientation) {
        orientation_pub_->publish(cmd);
    } else {
        position_pub_->publish(cmd);
    }

    return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus PublishToController::onRunning() { return BT::NodeStatus::SUCCESS; }

void PublishToController::onHalted() {}

} // namespace mercury_autonomy
