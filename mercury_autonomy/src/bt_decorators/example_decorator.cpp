// Example BT decorator node implementation.
// See the header for documentation on how to create new decorator nodes.

#include "mercury_autonomy/bt_decorators/example_decorator.hpp"

namespace mercury_autonomy
{

void ExampleDecorator::rosInit()
{
  // Create any ROS resources needed by this decorator.
  RCLCPP_DEBUG(rosNode()->get_logger(), "ExampleDecorator::rosInit()");
}

BT::NodeStatus ExampleDecorator::tick()
{
  int max_attempts = 3;
  getInput("max_attempts", max_attempts);

  setStatus(BT::NodeStatus::RUNNING);

  // Tick the child
  BT::NodeStatus child_status = child_node_->executeTick();

  switch (child_status) {
    case BT::NodeStatus::SUCCESS:
      // Child succeeded -- reset and propagate success.
      attempt_count_ = 0;
      return BT::NodeStatus::SUCCESS;

    case BT::NodeStatus::FAILURE:
      // Child failed -- retry up to max_attempts.
      attempt_count_++;
      if (attempt_count_ >= max_attempts) {
        attempt_count_ = 0;
        return BT::NodeStatus::FAILURE;
      }
      // Reset the child so it can try again on the next tick.
      haltChild();
      return BT::NodeStatus::RUNNING;

    case BT::NodeStatus::RUNNING:
      return BT::NodeStatus::RUNNING;

    default:
      return BT::NodeStatus::FAILURE;
  }
}

}  // namespace mercury_autonomy
