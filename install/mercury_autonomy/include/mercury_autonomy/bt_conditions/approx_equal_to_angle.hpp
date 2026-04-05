// ApproxEqualToAngle -- checks if two angles are approximately equal.
//
// Normalizes the angular difference to [-pi, pi] before comparing.
// Returns SUCCESS if the normalized |a - b| < range, FAILURE otherwise.
//
// Ports:
//   Input: a     -- First angle (radians).
//   Input: b     -- Second angle (radians).
//   Input: range -- Tolerance for comparison (radians).

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy
{

class ApproxEqualToAngle : public MercuryConditionNode
{
public:
  ApproxEqualToAngle(const std::string & name, const BT::NodeConfig & config)
  : MercuryConditionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<double>("a", "First angle (radians)"),
      BT::InputPort<double>("b", "Second angle (radians)"),
      BT::InputPort<double>("range", "Tolerance (radians)"),
    };
  }

  BT::NodeStatus tick() override;

protected:
  void rosInit() override;
};

}  // namespace mercury_autonomy
