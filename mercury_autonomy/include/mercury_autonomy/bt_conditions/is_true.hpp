// IsTrue -- checks if a boolean value is true.
//
// Returns SUCCESS if value is true, FAILURE otherwise.
//
// Ports:
//   Input: value -- Boolean value to check.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy
{

class IsTrue : public MercuryConditionNode
{
public:
  IsTrue(const std::string & name, const BT::NodeConfig & config)
  : MercuryConditionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<bool>("value", "Boolean value to check"),
    };
  }

  BT::NodeStatus tick() override;

protected:
  void rosInit() override;
};

}  // namespace mercury_autonomy
