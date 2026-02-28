// ApproxEqualTo -- checks if two values are approximately equal.
//
// Returns SUCCESS if |a - b| < range, FAILURE otherwise.
//
// Ports:
//   Input: a     -- First value.
//   Input: b     -- Second value.
//   Input: range -- Tolerance for comparison.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy
{

class ApproxEqualTo : public MercuryConditionNode
{
public:
  ApproxEqualTo(const std::string & name, const BT::NodeConfig & config)
  : MercuryConditionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<double>("a", "First value"),
      BT::InputPort<double>("b", "Second value"),
      BT::InputPort<double>("range", "Tolerance for approximate equality"),
    };
  }

  BT::NodeStatus tick() override;

protected:
  void rosInit() override;
};

}  // namespace mercury_autonomy
