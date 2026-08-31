// Example BT condition node implementation.
// See the header for documentation on how to create new condition nodes.

#include "mercury_autonomy/bt_conditions/example_condition.hpp"

namespace mercury_autonomy {

void ExampleCondition::rosInit() {
    // Create any ROS subscriptions needed to evaluate this condition.
    // Example:
    //   my_sub_ = rosNode()->create_subscription<std_msgs::msg::Float64>(
    //     "topic", 10, [this](std_msgs::msg::Float64::SharedPtr msg) { ... });
    RCLCPP_DEBUG(rosNode()->get_logger(), "ExampleCondition::rosInit()");
}

BT::NodeStatus ExampleCondition::tick() {
    // Read input ports
    double threshold = 0.5;
    getInput("threshold", threshold);

    // TODO: Replace with actual condition logic (e.g., check a FOG value).
    RCLCPP_DEBUG(rosNode()->get_logger(), "ExampleCondition checking threshold: %f", threshold);

    // Return SUCCESS if the condition is met, FAILURE otherwise.
    return BT::NodeStatus::SUCCESS;
}

} // namespace mercury_autonomy
