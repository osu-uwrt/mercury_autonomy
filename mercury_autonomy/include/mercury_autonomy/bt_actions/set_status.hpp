// SetStatus -- publishes a LED status command.
//
// Ports:
//   Input: status -- Named tree status mapped to a LED color/mode preset.

#pragma once

#include <mercury_msgs/msg/led_command.hpp>

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class SetStatus : public MercuryActionNode {
public:
    SetStatus(const std::string & name, const BT::NodeConfig & config)
        : MercuryActionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {BT::InputPort<std::string>("status", "Behavior status name")};
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

protected:
    void rosInit() override;

private:
    rclcpp::Publisher<mercury_msgs::msg::LedCommand>::SharedPtr pub_;
};

}  // namespace mercury_autonomy
