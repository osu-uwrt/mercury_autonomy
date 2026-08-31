// Example BT condition node -- use this as a template for new conditions.
//
// A condition node checks a predicate and returns SUCCESS or FAILURE
// in a single tick (synchronous). It never returns RUNNING.
//
// To create a new condition node:
//   1. Copy this header and the corresponding .cpp file.
//   2. Rename the class and update providedPorts().
//   3. Register the node in src/register_conditions.cpp.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class ExampleCondition : public MercuryConditionNode {
public:
    ExampleCondition(const std::string & name, const BT::NodeConfig & config)
        : MercuryConditionNode(name, config) {}

    /// Declare the ports (inputs / outputs) exposed by this node.
    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<double>("threshold", 0.5, "Threshold value to check against"),
        };
    }

    /// Evaluate the condition. Must return SUCCESS or FAILURE (never RUNNING).
    BT::NodeStatus tick() override;

protected:
    /// Create ROS subscriptions/publishers needed by this condition.
    void rosInit() override;
};

}  // namespace mercury_autonomy
