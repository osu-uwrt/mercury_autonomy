// IsTrue -- returns SUCCESS if the input boolean is true.

#include "mercury_autonomy/bt_conditions/is_true.hpp"

namespace mercury_autonomy {

void IsTrue::rosInit() {
    // No ROS resources needed.
}

BT::NodeStatus IsTrue::tick() {
    auto value = getInput<bool>("value");

    if (!value) {
        RCLCPP_ERROR(rosNode()->get_logger(), "IsTrue: missing required input [value]");
        return BT::NodeStatus::FAILURE;
    }

    return value.value() ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

}  // namespace mercury_autonomy
