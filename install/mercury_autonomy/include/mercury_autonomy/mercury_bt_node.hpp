// Base classes for custom BT nodes with ROS2 integration.
//
// Each Mercury BT node inherits from both a BT.CPP base class and
// MercuryBtNode, which provides shared access to a ROS2 node handle,
// a TF2 buffer, and convenience helpers.

#pragma once

#include "mercury_autonomy/autonomy_base.hpp"

namespace mercury_autonomy
{

/// Provides ROS2 integration for any custom BT node.
/// Subclasses must implement rosInit() for node-specific setup.
class MercuryBtNode
{
public:
  virtual ~MercuryBtNode() = default;

  /// One-time static initialization (TF buffer / listener).
  /// Called once before a tree is executed.
  static void staticInit(rclcpp::Node::SharedPtr node);

  /// Tear down static resources (call when tree execution ends).
  static void staticDeinit();

  /// Per-node initialization -- gives this node a ROS handle.
  void init(rclcpp::Node::SharedPtr node);

  /// Access the ROS node handle.
  rclcpp::Node::SharedPtr rosNode() const {return ros_node_;}

  /// Access the underlying BT TreeNode (implemented by each subclass).
  virtual BT::TreeNode * treeNode() = 0;

protected:
  /// Override this to create subscriptions, publishers, etc.
  virtual void rosInit() = 0;

  /// Look up a TF2 transform. Returns true on success.
  bool lookupTransform(
    const std::string & from_frame,
    const std::string & to_frame,
    geometry_msgs::msg::TransformStamped & transform,
    bool use_current_time = false);

  /// Shared TF2 buffer (created once in staticInit).
  static std::shared_ptr<tf2_ros::Buffer> tf_buffer_;

  /// The ROS2 node handle for this BT node.
  rclcpp::Node::SharedPtr ros_node_;

private:
  static std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
};

// ---------------------------------------------------------------------------
// Mercury-specific base classes for the three BT node categories
// ---------------------------------------------------------------------------

/// Base class for custom BT action nodes with ROS2 access.
class MercuryActionNode : public BT::StatefulActionNode, public MercuryBtNode
{
public:
  MercuryActionNode(const std::string & name, const BT::NodeConfig & config)
  : StatefulActionNode(name, config) {}

  BT::TreeNode * treeNode() override {return this;}
};

/// Base class for custom BT condition nodes with ROS2 access.
class MercuryConditionNode : public BT::ConditionNode, public MercuryBtNode
{
public:
  MercuryConditionNode(const std::string & name, const BT::NodeConfig & config)
  : ConditionNode(name, config) {}

  BT::TreeNode * treeNode() override {return this;}
};

/// Base class for custom BT decorator nodes with ROS2 access.
class MercuryDecoratorNode : public BT::DecoratorNode, public MercuryBtNode
{
public:
  MercuryDecoratorNode(const std::string & name, const BT::NodeConfig & config)
  : DecoratorNode(name, config) {}

  BT::TreeNode * treeNode() override {return this;}
};

}  // namespace mercury_autonomy
