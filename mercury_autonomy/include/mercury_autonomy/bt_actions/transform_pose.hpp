// TransformPose -- looks up a TF2 transform and applies it to an input pose.
//
// Takes an input pose (x, y, z, roll, pitch, yaw) in from_frame,
// transforms it to to_frame, and outputs the result.
//
// Ports:
//   Input:  from_frame -- Source frame.
//   Input:  to_frame   -- Target frame.
//   Input:  x, y, z    -- Position components.
//   Input:  roll, pitch, yaw -- Orientation components (radians).
//   Input:  timeout    -- Timeout in seconds (default 3.0).
//   Output: out_x, out_y, out_z -- Transformed position.
//   Output: out_roll, out_pitch, out_yaw -- Transformed orientation.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy {

class TransformPose : public MercuryActionNode {
public:
    TransformPose(const std::string & name, const BT::NodeConfig & config)
        : MercuryActionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {
            BT::InputPort<std::string>("from_frame", "Source TF frame"),
            BT::InputPort<std::string>("to_frame", "Target TF frame"),
            BT::InputPort<double>("x", 0.0, "Position X"),
            BT::InputPort<double>("y", 0.0, "Position Y"),
            BT::InputPort<double>("z", 0.0, "Position Z"),
            BT::InputPort<double>("roll", 0.0, "Orientation roll (rad)"),
            BT::InputPort<double>("pitch", 0.0, "Orientation pitch (rad)"),
            BT::InputPort<double>("yaw", 0.0, "Orientation yaw (rad)"),
            BT::InputPort<double>("timeout", 3.0, "Timeout in seconds"),
            BT::OutputPort<double>("out_x", "Transformed X"),
            BT::OutputPort<double>("out_y", "Transformed Y"),
            BT::OutputPort<double>("out_z", "Transformed Z"),
            BT::OutputPort<double>("out_roll", "Transformed roll"),
            BT::OutputPort<double>("out_pitch", "Transformed pitch"),
            BT::OutputPort<double>("out_yaw", "Transformed yaw"),
        };
    }

    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

protected:
    void rosInit() override;

private:
    std::string from_frame_;
    std::string to_frame_;
    geometry_msgs::msg::Pose original_;
    rclcpp::Time start_time_;
    double timeout_sec_ = 3.0;
};

} // namespace mercury_autonomy
