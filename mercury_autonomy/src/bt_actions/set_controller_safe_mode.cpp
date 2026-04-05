// SetControllerSafeMode -- updates the controller safe-mode parameter asynchronously.

#include "mercury_autonomy/bt_actions/set_controller_safe_mode.hpp"

namespace mercury_autonomy
{

namespace
{
std::string find_controller_node(const rclcpp::Node::SharedPtr & node)
{
  for (const auto & node_name : node->get_node_names()) {
    if (node_name.rfind("/complete_controller", 0) == 0 ||
      node_name.rfind("complete_controller", 0) == 0)
    {
      return node_name;
    }
  }
  return "complete_controller";
}
}  // namespace

void SetControllerSafeMode::rosInit()
{
  async_client_ = std::make_shared<rclcpp::AsyncParametersClient>(
    rosNode(), find_controller_node(rosNode()));
}

BT::NodeStatus SetControllerSafeMode::onStart()
{
  bool safe = true;
  getInput("safe", safe);
  getInput("timeout_secs", timeout_secs_);

  set_complete_ = false;
  std::vector<rclcpp::Parameter> params;
  params.emplace_back("controller__stunt__safe_mode", safe);

  async_client_->set_parameters(
    params, std::bind(&SetControllerSafeMode::onSetParametersDone, this, std::placeholders::_1));
  start_time_ = rosNode()->now();
  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus SetControllerSafeMode::onRunning()
{
  if (set_complete_) {
    return BT::NodeStatus::SUCCESS;
  }

  if ((rosNode()->now() - start_time_).seconds() > timeout_secs_) {
    RCLCPP_ERROR(rosNode()->get_logger(), "SetControllerSafeMode: timed out setting parameter");
    return BT::NodeStatus::FAILURE;
  }

  return BT::NodeStatus::RUNNING;
}

void SetControllerSafeMode::onHalted()
{
}

void SetControllerSafeMode::onSetParametersDone(
  std::shared_future<std::vector<rcl_interfaces::msg::SetParametersResult>> future)
{
  auto results = future.get();
  if (results.size() != 1) {
    RCLCPP_ERROR(
      rosNode()->get_logger(),
      "SetControllerSafeMode: unexpected parameter result count");
    return;
  }

  if (results.front().successful) {
    set_complete_ = true;
  } else {
    RCLCPP_ERROR(
      rosNode()->get_logger(), "SetControllerSafeMode: parameter update failed: %s",
      results.front().reason.c_str());
  }
}

}  // namespace mercury_autonomy
