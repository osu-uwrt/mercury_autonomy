// RetryUntilSuccessfulOrTimeout -- retries child until success or timeout.
//
// Continuously ticks the child node. If the child returns SUCCESS, the
// decorator succeeds. If the child returns FAILURE, it is halted and
// retried on the next tick. If the total elapsed time exceeds the
// configured timeout, the decorator fails.
//
// Ports:
//   Input: timeout -- Maximum time in seconds before giving up.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class RetryUntilSuccessfulOrTimeout : public MercuryDecoratorNode {
public:
    RetryUntilSuccessfulOrTimeout(const std::string & name, const BT::NodeConfig & config)
        : MercuryDecoratorNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<double>("timeout", "Maximum time in seconds"),
        };
    }

    BT::NodeStatus tick() override;

protected:
    void rosInit() override;

private:
    double timeout_sec_ = 0.0;
    rclcpp::Time start_time_;
    bool started_ = false;
};

}  // namespace mercury_autonomy
