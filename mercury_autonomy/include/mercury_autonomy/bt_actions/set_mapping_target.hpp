// SetMappingTarget -- calls the mapping target service with a target object.
//
// Ports:
//   Input: target_object   -- Mapping target object identifier.
//   Input: lock_map        -- Whether mapping should remain locked.
//   Input: time_limit_secs -- Service timeout.

#pragma once

#include <mercury_msgs/srv/mapping_target.hpp>

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class SetMappingTarget : public MercuryActionNode {
public:
    SetMappingTarget(const std::string & name, const BT::NodeConfig & config)
        : MercuryActionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("target_object", "Target object name"),
            BT::InputPort<bool>("lock_map", false, "Lock or unlock the map"),
            BT::InputPort<double>("time_limit_secs", 0.0, "Timeout in seconds"),
        };
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

protected:
    void rosInit() override;

private:
    rclcpp::Client<mercury_msgs::srv::MappingTarget>::SharedPtr client_;
    rclcpp::Client<mercury_msgs::srv::MappingTarget>::SharedFuture future_;
    rclcpp::Time start_time_;
    double timeout_secs_ = 0.0;
};

}  // namespace mercury_autonomy
