// ApproxEqualToAngle -- angle-aware approximate equality check.

#include "mercury_autonomy/bt_conditions/approx_equal_to_angle.hpp"

#include <cmath>

namespace mercury_autonomy {

void ApproxEqualToAngle::rosInit() {
    // No ROS resources needed for a pure comparison.
}

BT::NodeStatus ApproxEqualToAngle::tick() {
    auto a = getInput<double>("a");
    auto b = getInput<double>("b");
    auto range = getInput<double>("range");

    if (!a || !b || !range) {
        RCLCPP_ERROR(
            rosNode()->get_logger(),
            "ApproxEqualToAngle: missing required input (a=%s, b=%s, range=%s)",
            a ? "ok" : "MISSING", b ? "ok" : "MISSING", range ? "ok" : "MISSING");
        return BT::NodeStatus::FAILURE;
    }

    // Compute difference and wrap to [-pi, pi]
    double diff = a.value() - b.value();
    diff = std::fmod(diff, 2.0 * M_PI);
    if (diff > M_PI) {
        diff -= 2.0 * M_PI;
    } else if (diff < -M_PI) {
        diff += 2.0 * M_PI;
    }

    bool result = std::abs(diff) < range.value();

    RCLCPP_DEBUG(
        rosNode()->get_logger(), "ApproxEqualToAngle: wrapped |%.4f - %.4f| = %.4f %s %.4f -> %s",
        a.value(), b.value(), std::abs(diff), result ? "<" : ">=", range.value(),
        result ? "SUCCESS" : "FAILURE");

    return result ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

} // namespace mercury_autonomy
