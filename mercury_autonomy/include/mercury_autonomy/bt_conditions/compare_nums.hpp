// CompareNums -- numeric comparison condition node.
//
// Compares two numeric values using a specified operator.
// Returns SUCCESS if the comparison holds, FAILURE otherwise.
//
// Ports:
//   Input: a    -- Left-hand operand.
//   Input: b    -- Right-hand operand.
//   Input: test -- Comparison operator: ">", "<", or "==".

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class CompareNums : public MercuryConditionNode {
public:
    CompareNums(const std::string & name, const BT::NodeConfig & config)
        : MercuryConditionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("test", "Comparison operator: >, <, or =="),
            BT::InputPort<double>("a", "Left-hand operand"),
            BT::InputPort<double>("b", "Right-hand operand"),
        };
    }

    BT::NodeStatus tick() override;

protected:
    void rosInit() override;
};

} // namespace mercury_autonomy
