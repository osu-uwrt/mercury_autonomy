// CallSetBoolService -- calls a std_srvs/SetBool service asynchronously.
//
// Ports:
//   Input: srv_name       -- Service name to call.
//   Input: data           -- Boolean data to send in the request.
//   Input: timeout        -- Timeout in seconds (default 3.0).

#pragma once

#include <optional>

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class CallSetBoolService : public MercuryActionNode {
public:
    CallSetBoolService(const std::string & name, const BT::NodeConfig & config)
        : MercuryActionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("srv_name", "Service name to call"),
            BT::InputPort<bool>("data", "Boolean value to send"),
            BT::InputPort<double>("timeout", 3.0, "Timeout in seconds"),
        };
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

protected:
    void rosInit() override;

private:
    using SetBool = std_srvs::srv::SetBool;
    using FutureAndId = rclcpp::Client<SetBool>::FutureAndRequestId;

    std::string srv_name_;
    rclcpp::Client<SetBool>::SharedPtr client_;
    std::optional<FutureAndId> result_;
    rclcpp::Time start_time_;
    double timeout_sec_ = 3.0;
};

}  // namespace mercury_autonomy
