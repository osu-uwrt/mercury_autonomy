// GetMappingState -- returns the current mapping target and lock state.

#include "mercury_autonomy/bt_actions/get_mapping_state.hpp"

namespace mercury_autonomy
{

namespace
{
constexpr char kMappingStateTopic[] = "state/mapping";
}

void GetMappingState::rosInit()
{
  sub_ = rosNode()->create_subscription<mercury_msgs::msg::MappingTargetInfo>(
    kMappingStateTopic, 10,
    [this](mercury_msgs::msg::MappingTargetInfo::SharedPtr msg) {
      latest_msg_ = *msg;
      msg_received_ = true;
    });
}

BT::NodeStatus GetMappingState::onStart()
{
  msg_received_ = false;
  start_time_ = rosNode()->now();
  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus GetMappingState::onRunning()
{
  if (msg_received_) {
    setOutput("map_locked", latest_msg_.lock_map);
    setOutput("target_name", latest_msg_.target_object);
    return BT::NodeStatus::SUCCESS;
  }

  if ((rosNode()->now() - start_time_).seconds() > 3.0) {
    RCLCPP_ERROR(rosNode()->get_logger(), "GetMappingState: timed out waiting for mapping state");
    setOutput("map_locked", false);
    setOutput("target_name", std::string{});
    return BT::NodeStatus::FAILURE;
  }

  return BT::NodeStatus::RUNNING;
}

void GetMappingState::onHalted()
{
}

}  // namespace mercury_autonomy
