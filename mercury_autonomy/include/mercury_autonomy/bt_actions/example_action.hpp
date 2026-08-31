// Example BT action node -- use this as a template for new actions.
//
// An action node performs work over multiple ticks. The three callbacks
// (onStart, onRunning, onHalted) map to the BT.CPP v4 StatefulActionNode
// lifecycle.
//
// To create a new action node:
//   1. Copy this header and the corresponding .cpp file.
//   2. Rename the class and update providedPorts().
//   3. Register the node in src/register_actions.cpp.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class ExampleAction : public MercuryActionNode {
public:
    ExampleAction(const std::string & name, const BT::NodeConfig & config)
        : MercuryActionNode(name, config) {}

    /// Declare the ports (inputs / outputs) exposed by this node.
    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("target", "Description of the target input"),
            BT::OutputPort<std::string>("result", "Description of the result output"),
        };
    }

    /// Called once when the node transitions from IDLE to RUNNING.
    BT::NodeStatus onStart() override;

    /// Called on every subsequent tick while the node is RUNNING.
    BT::NodeStatus onRunning() override;

    /// Called when the node is halted (e.g., tree cancelled).
    void onHalted() override;

protected:
    /// Create ROS subscriptions/publishers needed by this action.
    void rosInit() override;
};

} // namespace mercury_autonomy
