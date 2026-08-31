// Example BT action node implementation.
// See the header for documentation on how to create new action nodes.

#include "mercury_autonomy/bt_actions/example_action.hpp"

namespace mercury_autonomy {

void ExampleAction::rosInit() {
    // Create any ROS publishers, subscribers, or service clients here.
    // Example:
    //   my_pub_ = rosNode()->create_publisher<std_msgs::msg::String>("topic", 10);
    RCLCPP_DEBUG(rosNode()->get_logger(), "ExampleAction::rosInit()");
}

BT::NodeStatus ExampleAction::onStart() {
    // Read input ports
    auto target = getInput<std::string>("target");
    if (!target) {
        RCLCPP_ERROR(rosNode()->get_logger(), "ExampleAction: missing required input [target]");
        return BT::NodeStatus::FAILURE;
    }

    RCLCPP_INFO(
        rosNode()->get_logger(), "ExampleAction started with target: %s", target.value().c_str());

    // TODO: Begin the action (e.g., send a goal to a ROS action server).
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus ExampleAction::onRunning() {
    // TODO: Check progress. Return RUNNING to keep ticking, SUCCESS or FAILURE to finish.
    RCLCPP_DEBUG(rosNode()->get_logger(), "ExampleAction running...");
    return BT::NodeStatus::SUCCESS;
}

void ExampleAction::onHalted() {
    // TODO: Clean up resources (e.g., cancel a pending ROS action goal).
    RCLCPP_INFO(rosNode()->get_logger(), "ExampleAction halted.");
}

} // namespace mercury_autonomy
