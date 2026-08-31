// Example BT decorator node -- use this as a template for new decorators.
//
// A decorator wraps a single child node and can modify its behavior
// (e.g., repeat, timeout, invert). The tick() method controls when
// and how the child is ticked.
//
// To create a new decorator node:
//   1. Copy this header and the corresponding .cpp file.
//   2. Rename the class and update providedPorts().
//   3. Register the node in src/register_decorators.cpp.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class ExampleDecorator : public MercuryDecoratorNode {
public:
    ExampleDecorator(const std::string & name, const BT::NodeConfig & config)
        : MercuryDecoratorNode(name, config) {}

    /// Declare the ports (inputs / outputs) exposed by this node.
    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<int>("max_attempts", 3, "Maximum number of child retries"),
        };
    }

    /// Tick the decorator. Must tick the child and return the result.
    BT::NodeStatus tick() override;

protected:
    /// Create ROS subscriptions/publishers if needed.
    void rosInit() override;

private:
    int attempt_count_ = 0;
};

}  // namespace mercury_autonomy
