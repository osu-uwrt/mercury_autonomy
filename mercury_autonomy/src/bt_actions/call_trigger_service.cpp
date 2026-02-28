// CallTriggerService -- asynchronously calls a Trigger service.

#include "mercury_autonomy/bt_actions/call_trigger_service.hpp"

namespace mercury_autonomy
{

void CallTriggerService::rosInit()
{
  // Client is created in onStart so the service name can be read from ports.
}

BT::NodeStatus CallTriggerService::onStart()
{
  auto srv = getInput<std::string>("srv_name");
  if (!srv) {
    RCLCPP_ERROR(rosNode()->get_logger(), "CallTriggerService: missing [srv_name]");
    return BT::NodeStatus::FAILURE;
  }
  srv_name_ = srv.value();

  getInput("timeout", timeout_sec_);

  client_ = rosNode()->create_client<Trigger>(srv_name_);
  if (!client_->wait_for_service(std::chrono::seconds(1))) {
    RCLCPP_ERROR(
      rosNode()->get_logger(), "CallTriggerService: service %s not available",
      srv_name_.c_str());
    return BT::NodeStatus::FAILURE;
  }

  auto request = std::make_shared<Trigger::Request>();
  result_.emplace(client_->async_send_request(request));
  start_time_ = rosNode()->now();

  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus CallTriggerService::onRunning()
{
  if (!result_.has_value() || !result_->valid()) {
    RCLCPP_ERROR(
      rosNode()->get_logger(), "CallTriggerService: future invalid for %s",
      srv_name_.c_str());
    return BT::NodeStatus::FAILURE;
  }

  if (result_->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
    auto resp = result_->get();
    if (!resp->message.empty() && !resp->success) {
      RCLCPP_WARN(
        rosNode()->get_logger(), "CallTriggerService %s: %s",
        srv_name_.c_str(), resp->message.c_str());
    }
    return resp->success ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
  }

  double elapsed = (rosNode()->now() - start_time_).seconds();
  if (elapsed > timeout_sec_) {
    RCLCPP_ERROR(
      rosNode()->get_logger(), "CallTriggerService: %s timed out after %.1f s",
      srv_name_.c_str(), timeout_sec_);
    return BT::NodeStatus::FAILURE;
  }

  return BT::NodeStatus::RUNNING;
}

void CallTriggerService::onHalted()
{
  result_.reset();
  client_.reset();
}

}  // namespace mercury_autonomy
