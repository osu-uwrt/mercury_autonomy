// Utility functions: plugin loading, ROS-BT init, geometry helpers.

#include "mercury_autonomy/autonomy_util.hpp"

#include <cmath>
#include <cstdlib>
#include <stdexcept>

namespace mercury_autonomy
{

// Plugin management

void registerPlugins(
  std::shared_ptr<BT::BehaviorTreeFactory> factory,
  const std::string & package_name)
{
  const std::string prefix = ament_index_cpp::get_package_prefix(package_name);
  factory->registerFromPlugin(prefix + "/lib/libmercury_bt_actions.so");
  factory->registerFromPlugin(prefix + "/lib/libmercury_bt_conditions.so");
  factory->registerFromPlugin(prefix + "/lib/libmercury_bt_decorators.so");
}

void initRosForTree(BT::Tree & tree, rclcpp::Node::SharedPtr ros_node)
{
  // Set up shared static resources (TF buffer, etc.)
  MercuryBtNode::staticInit(ros_node);

  // Expose the robot namespace on every subtree's blackboard so BT XML
  // authors can reference it (e.g., for TF frame names: "{robot_ns}/base_link").
  // The namespace is the ROS node's namespace stripped of its leading slash.
  std::string ns = ros_node->get_namespace();
  if (!ns.empty() && ns.front() == '/') {
    ns = ns.substr(1);
  }
  if (ns.empty()) {
    ns = "mercury";  // default when no namespace is set
  }
  for (auto & subtree : tree.subtrees) {
    subtree->blackboard->set("robot_ns", ns);
  }

  // Hand each custom node a ROS handle so it can create pubs/subs.
  for (auto & node : tree.subtrees) {
    for (auto & tree_node : node->nodes) {
      if (auto * mercury_node = dynamic_cast<MercuryBtNode *>(tree_node.get())) {
        mercury_node->init(ros_node);
      }
    }
  }
}

// Environment helpers

std::string getEnvVar(const char * name)
{
  const char * value = std::getenv(name);
  if (!value) {
    throw std::runtime_error(std::string("Environment variable not set: ") + name);
  }
  return std::string(value);
}

// Geometry helpers

geometry_msgs::msg::Pose doTransform(
  const geometry_msgs::msg::Pose & pose,
  const geometry_msgs::msg::TransformStamped & transform)
{
  geometry_msgs::msg::Pose result;
  tf2::doTransform(pose, result, transform);
  return result;
}

geometry_msgs::msg::Vector3 toRPY(const geometry_msgs::msg::Quaternion & quat)
{
  tf2::Quaternion tf2_quat;
  tf2::fromMsg(quat, tf2_quat);

  geometry_msgs::msg::Vector3 rpy;
  tf2::Matrix3x3(tf2_quat).getEulerYPR(rpy.z, rpy.y, rpy.x);
  return rpy;
}

geometry_msgs::msg::Quaternion toQuat(const geometry_msgs::msg::Vector3 & rpy)
{
  tf2::Quaternion tf2_quat;
  tf2_quat.setRPY(rpy.x, rpy.y, rpy.z);
  tf2_quat.normalize();
  return tf2::toMsg(tf2_quat);
}

double distance(
  const geometry_msgs::msg::Point & a,
  const geometry_msgs::msg::Point & b)
{
  return std::sqrt(
    std::pow(b.x - a.x, 2) +
    std::pow(b.y - a.y, 2) +
    std::pow(b.z - a.z, 2));
}

}  // namespace mercury_autonomy
