// Wait -- returns SUCCESS once the configured duration has elapsed.

#include "mercury_autonomy/bt_actions/wait.hpp"

namespace mercury_autonomy
{

void Wait::rosInit()
{
}

BT::NodeStatus Wait::onStart()
{
  getInput("seconds", goal_time_);
  start_time_ = rosNode()->now();
  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus Wait::onRunning()
{
  return ((rosNode()->now() - start_time_).seconds() >= goal_time_) ?
         BT::NodeStatus::SUCCESS : BT::NodeStatus::RUNNING;
}

void Wait::onHalted()
{
}

}  // namespace mercury_autonomy
