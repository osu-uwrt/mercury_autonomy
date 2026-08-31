// SetFeedforwardMode -- sets the controller to feed forward mode for both linear and angular
// controllers.
//
// Ports:
//   None.

#pragma once

#include <mercury_msgs/msg/controller_command.hpp>
#include <std_srvs/srv/set_bool.hpp>

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class SetFeedforwardMode : public MercuryActionNode {
public:
    SetFeedforwardMode(const std::string & name, const BT::NodeConfig & config)
        : MercuryActionNode(name, config) {}

    static BT::PortsList providedPorts() { return {}; }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

protected:
    void rosInit() override;

private:
    using SetBool = std_srvs::srv::SetBool;

    rclcpp::Publisher<mercury_msgs::msg::ControllerCommand>::SharedPtr linear_pub_;
    rclcpp::Publisher<mercury_msgs::msg::ControllerCommand>::SharedPtr angular_pub_;
    rclcpp::Client<SetBool>::SharedPtr set_teleop_client_;
};

} // namespace mercury_autonomy
