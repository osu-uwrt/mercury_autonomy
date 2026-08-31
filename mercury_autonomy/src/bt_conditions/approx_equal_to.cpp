// ApproxEqualTo -- returns SUCCESS if |a - b| < range.

#include "mercury_autonomy/bt_conditions/approx_equal_to.hpp"

#include <cmath>

namespace mercury_autonomy {

void ApproxEqualTo::rosInit() {
    // No ROS resources needed for a pure comparison.
}

BT::NodeStatus ApproxEqualTo::tick() {
    auto a = getInput<double>("a");
    auto b = getInput<double>("b");
    auto range = getInput<double>("range");

    if (!a || !b || !range) {
        RCLCPP_ERROR(
            rosNode()->get_logger(), "ApproxEqualTo: missing required input (a=%s, b=%s, range=%s)",
            a ? "ok" : "MISSING", b ? "ok" : "MISSING", range ? "ok" : "MISSING");
        return BT::NodeStatus::FAILURE;
    }

    bool result = std::abs(a.value() - b.value()) < range.value();

    RCLCPP_DEBUG(
        rosNode()->get_logger(), "ApproxEqualTo: |%.4f - %.4f| = %.4f %s %.4f -> %s", a.value(),
        b.value(), std::abs(a.value() - b.value()), result ? "<" : ">=", range.value(),
        result ? "SUCCESS" : "FAILURE");

    return result ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

}  // namespace mercury_autonomy
