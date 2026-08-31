// CallSetBoolService -- asynchronously calls a SetBool service.

#include "mercury_autonomy/bt_actions/call_set_bool_service.hpp"

namespace mercury_autonomy {

void CallSetBoolService::rosInit() {
    // Client is created in onStart so the service name can be read from ports.
}

BT::NodeStatus CallSetBoolService::onStart() {
    auto srv = getInput<std::string>("srv_name");
    if (!srv) {
        RCLCPP_ERROR(rosNode()->get_logger(), "CallSetBoolService: missing [srv_name]");
        return BT::NodeStatus::FAILURE;
    }
    srv_name_ = srv.value();

    auto data_opt = getInput<bool>("data");
    if (!data_opt) {
        RCLCPP_ERROR(rosNode()->get_logger(), "CallSetBoolService: missing [data]");
        return BT::NodeStatus::FAILURE;
    }

    getInput("timeout", timeout_sec_);

    client_ = rosNode()->create_client<SetBool>(srv_name_);
    if (!client_->wait_for_service(std::chrono::seconds(1))) {
        RCLCPP_ERROR(
            rosNode()->get_logger(), "CallSetBoolService: service %s not available",
            srv_name_.c_str());
        return BT::NodeStatus::FAILURE;
    }

    auto request = std::make_shared<SetBool::Request>();
    request->data = data_opt.value();
    result_.emplace(client_->async_send_request(request));
    start_time_ = rosNode()->now();

    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus CallSetBoolService::onRunning() {
    if (!result_.has_value() || !result_->valid()) {
        RCLCPP_ERROR(
            rosNode()->get_logger(), "CallSetBoolService: future invalid for %s",
            srv_name_.c_str());
        return BT::NodeStatus::FAILURE;
    }

    // Check if the response is ready
    if (result_->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        auto resp = result_->get();
        if (!resp->message.empty()) {
            RCLCPP_INFO(
                rosNode()->get_logger(), "CallSetBoolService %s: %s", srv_name_.c_str(),
                resp->message.c_str());
        }
        return resp->success ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
    }

    // Check timeout
    double elapsed = (rosNode()->now() - start_time_).seconds();
    if (elapsed > timeout_sec_) {
        RCLCPP_ERROR(
            rosNode()->get_logger(), "CallSetBoolService: %s timed out after %.1f s",
            srv_name_.c_str(), timeout_sec_);
        return BT::NodeStatus::FAILURE;
    }

    return BT::NodeStatus::RUNNING;
}

void CallSetBoolService::onHalted() {
    result_.reset();
    client_.reset();
}

} // namespace mercury_autonomy
