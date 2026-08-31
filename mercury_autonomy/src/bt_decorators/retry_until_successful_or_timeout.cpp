// RetryUntilSuccessfulOrTimeout -- retries child until it succeeds or time runs out.

#include "mercury_autonomy/bt_decorators/retry_until_successful_or_timeout.hpp"

namespace mercury_autonomy {

void RetryUntilSuccessfulOrTimeout::rosInit() {
    // No ROS resources needed.
}

BT::NodeStatus RetryUntilSuccessfulOrTimeout::tick() {
    // On first tick (transition from IDLE), record start time and read timeout
    if (status() == BT::NodeStatus::IDLE) {
        auto t = getInput<double>("timeout");
        if (!t) {
            RCLCPP_ERROR(
                rosNode()->get_logger(),
                "RetryUntilSuccessfulOrTimeout: missing required input [timeout]");
            return BT::NodeStatus::FAILURE;
        }
        timeout_sec_ = t.value();
        start_time_ = rosNode()->now();
        started_ = true;
    }

    double elapsed = (rosNode()->now() - start_time_).seconds();
    if (elapsed >= timeout_sec_) {
        RCLCPP_ERROR(
            rosNode()->get_logger(), "RetryUntilSuccessfulOrTimeout '%s' timed out after %.1f s",
            name().c_str(), timeout_sec_);
        haltChild();
        return BT::NodeStatus::FAILURE;
    }

    setStatus(BT::NodeStatus::RUNNING);
    BT::NodeStatus child_status = child_node_->executeTick();

    if (child_status == BT::NodeStatus::SUCCESS) {
        return BT::NodeStatus::SUCCESS;
    }

    if (child_status == BT::NodeStatus::FAILURE) {
        // Reset child so it can be retried on the next tick
        haltChild();
        return BT::NodeStatus::RUNNING;
    }

    // Child is still RUNNING
    return BT::NodeStatus::RUNNING;
}

} // namespace mercury_autonomy
