// Utility functions for the mercury_autonomy package.
//
// Provides plugin registration, ROS-BT tree initialization, and
// common geometric helpers.

#pragma once

#include "mercury_autonomy/autonomy_base.hpp"
#include "mercury_autonomy/mercury_bt_node.hpp"

namespace mercury_autonomy
{

// ---------------------------------------------------------------------------
// Plugin management
// ---------------------------------------------------------------------------

/// Register all mercury_autonomy BT plugin libraries with the factory.
/// Loads libmercury_bt_actions, libmercury_bt_conditions, and
/// libmercury_bt_decorators from the ament index install path.
void registerPlugins(
  std::shared_ptr<BT::BehaviorTreeFactory> factory,
  const std::string & package_name);

/// Give every MercuryBtNode in the tree a ROS node handle.
/// Must be called before ticking a tree that contains custom nodes.
void initRosForTree(BT::Tree & tree, rclcpp::Node::SharedPtr ros_node);

// ---------------------------------------------------------------------------
// Environment helpers
// ---------------------------------------------------------------------------

/// Retrieve an environment variable or throw if unset.
std::string getEnvVar(const char * name);

// ---------------------------------------------------------------------------
// Geometry helpers
// ---------------------------------------------------------------------------

/// Transform a pose by the given transform.
geometry_msgs::msg::Pose doTransform(
  const geometry_msgs::msg::Pose & pose,
  const geometry_msgs::msg::TransformStamped & transform);

/// Quaternion to roll-pitch-yaw (radians).
geometry_msgs::msg::Vector3 toRPY(const geometry_msgs::msg::Quaternion & quat);

/// Roll-pitch-yaw (radians) to quaternion.
geometry_msgs::msg::Quaternion toQuat(const geometry_msgs::msg::Vector3 & rpy);

/// Euclidean distance between two points.
double distance(
  const geometry_msgs::msg::Point & a,
  const geometry_msgs::msg::Point & b);

}  // namespace mercury_autonomy
