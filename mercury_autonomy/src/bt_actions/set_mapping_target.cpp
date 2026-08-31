// SetMappingTarget -- calls the mapping target service and waits for completion.

#include "mercury_autonomy/bt_actions/set_mapping_target.hpp"

namespace mercury_autonomy {

namespace {
constexpr char kMappingServiceName[] = "mapping_target";
}

void SetMappingTarget::rosInit() {
    client_ = rosNode()->create_client<mercury_msgs::srv::MappingTarget>(kMappingServiceName);
}

BT::NodeStatus SetMappingTarget::onStart() {
    getInput("time_limit_secs", timeout_secs_);

    if (!client_->wait_for_service(std::chrono::seconds(1))) {
        RCLCPP_ERROR(
            rosNode()->get_logger(), "SetMappingTarget: service %s is not available",
            kMappingServiceName);
        return BT::NodeStatus::FAILURE;
    }

    std::string target_object;
    getInput("target_object", target_object);

    bool lock_map = false;
    getInput("lock_map", lock_map);

    auto request = std::make_shared<mercury_msgs::srv::MappingTarget::Request>();
    // Keep request fields aligned with mapping state semantics used by GetMappingState.
    request->target_info.target_object = target_object;
    request->target_info.lock_map = lock_map;

    auto response_future = client_->async_send_request(request);
    future_ = response_future.future.share();
    start_time_ = rosNode()->now();
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus SetMappingTarget::onRunning() {
    if (!future_.valid()) {
        RCLCPP_ERROR(rosNode()->get_logger(), "SetMappingTarget: service future is invalid");
        return BT::NodeStatus::FAILURE;
    }

    if (future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        future_.get();
        return BT::NodeStatus::SUCCESS;
    }

    if ((rosNode()->now() - start_time_).seconds() > timeout_secs_) {
        RCLCPP_ERROR(
            rosNode()->get_logger(), "SetMappingTarget: timed out calling %s", kMappingServiceName);
        return BT::NodeStatus::FAILURE;
    }

    return BT::NodeStatus::RUNNING;
}

void SetMappingTarget::onHalted() {}

}  // namespace mercury_autonomy
