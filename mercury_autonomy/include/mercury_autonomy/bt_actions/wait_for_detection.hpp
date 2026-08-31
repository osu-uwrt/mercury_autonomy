// WaitForDetection -- waits for a matching detection message.
//
// Ports:
//   Input: object_name  -- Target detection class id.
//   Input: timeout_secs -- Maximum wait duration.

#pragma once

#include <vision_msgs/msg/detection3_d_array.hpp>

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class WaitForDetection : public MercuryActionNode {
public:
    WaitForDetection(const std::string & name, const BT::NodeConfig & config)
        : MercuryActionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("object_name", "Target class id"),
            BT::InputPort<double>("timeout_secs", 0.0, "Timeout in seconds"),
        };
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

protected:
    void rosInit() override;

private:
    void detectionCallback(const vision_msgs::msg::Detection3DArray::SharedPtr msg);

    rclcpp::Subscription<vision_msgs::msg::Detection3DArray>::SharedPtr sub_;
    bool found_object_ = false;
    std::string target_object_id_;
    double timeout_secs_ = 0.0;
    rclcpp::Time start_time_;
};

}  // namespace mercury_autonomy
