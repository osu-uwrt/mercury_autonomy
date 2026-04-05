// ComputeFrameAlignment -- computes a world-aligned controller pose from TF data.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy
{

class ComputeFrameAlignment : public MercuryActionNode
{
public:
  ComputeFrameAlignment(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {}

  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<double>("x", 0.0, "Input X"),
      BT::InputPort<double>("y", 0.0, "Input Y"),
      BT::InputPort<double>("z", 0.0, "Input Z"),
      BT::InputPort<double>("or", 0.0, "Input roll"),
      BT::InputPort<double>("op", 0.0, "Input pitch"),
      BT::InputPort<double>("oy", 0.0, "Input yaw"),
      BT::InputPort<std::string>("reference_frame", "Reference TF frame"),
      BT::InputPort<std::string>("link_frame", "Link TF frame"),
      BT::InputPort<std::string>("base_frame", "Base TF frame"),
      BT::OutputPort<double>("out_x", "Aligned X"),
      BT::OutputPort<double>("out_y", "Aligned Y"),
      BT::OutputPort<double>("out_z", "Aligned Z"),
      BT::OutputPort<double>("out_or", "Aligned roll"),
      BT::OutputPort<double>("out_op", "Aligned pitch"),
      BT::OutputPort<double>("out_oy", "Aligned yaw"),
    };
  }

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;

private:
  std::string reference_frame_;
  std::string link_frame_;
  std::string base_frame_;
  geometry_msgs::msg::TransformStamped input_pose_;
  geometry_msgs::msg::TransformStamped geom_ttb_;
  geometry_msgs::msg::TransformStamped geom_twr_;
  bool have_ttb_ = false;
  bool have_twr_ = false;
  rclcpp::Time start_time_;
};

}  // namespace mercury_autonomy
