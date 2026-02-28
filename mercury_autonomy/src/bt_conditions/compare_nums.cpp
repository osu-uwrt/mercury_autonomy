// CompareNums -- evaluates a numeric comparison between two values.

#include "mercury_autonomy/bt_conditions/compare_nums.hpp"

namespace mercury_autonomy
{

void CompareNums::rosInit()
{
  // No ROS resources needed.
}

BT::NodeStatus CompareNums::tick()
{
  auto test = getInput<std::string>("test");
  auto a = getInput<double>("a");
  auto b = getInput<double>("b");

  if (!test || !a || !b) {
    RCLCPP_ERROR(
      rosNode()->get_logger(),
      "CompareNums: missing required input (test=%s, a=%s, b=%s)",
      test ? "ok" : "MISSING", a ? "ok" : "MISSING", b ? "ok" : "MISSING");
    return BT::NodeStatus::FAILURE;
  }

  const std::string & op = test.value();
  bool result = false;

  if (op == ">") {
    result = a.value() > b.value();
  } else if (op == "<") {
    result = a.value() < b.value();
  } else if (op == "==") {
    result = a.value() == b.value();
  } else if (op == ">=") {
    result = a.value() >= b.value();
  } else if (op == "<=") {
    result = a.value() <= b.value();
  } else if (op == "!=") {
    result = a.value() != b.value();
  } else {
    RCLCPP_ERROR(
      rosNode()->get_logger(),
      "CompareNums: invalid operator '%s'. Use >, <, ==, >=, <=, or !=",
      op.c_str());
    return BT::NodeStatus::FAILURE;
  }

  RCLCPP_DEBUG(
    rosNode()->get_logger(),
    "CompareNums: %.4f %s %.4f -> %s",
    a.value(), op.c_str(), b.value(),
    result ? "SUCCESS" : "FAILURE");

  return result ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

}  // namespace mercury_autonomy
