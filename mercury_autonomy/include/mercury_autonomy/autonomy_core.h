#ifndef MERCURY_AUTONOMY__AUTONOMY_CORE_H_
#define MERCURY_AUTONOMY__AUTONOMY_CORE_H_

#include <memory>
#include <string>
#include <vector>
#include <functional>

#include <rclcpp/rclcpp.hpp>
#include <behaviortree_cpp/bt_factory.h>

namespace mercury_autonomy
{

/**
 * @class AutonomyCore
 * @brief Core autonomy system for managing behavior trees
 *
 * This class provides the foundational functionality for managing and executing
 * BehaviorTree.CPP trees within a ROS2 environment. It handles tree loading,
 * execution, and lifecycle management.
 */
class AutonomyCore
{
public:
  /**
   * @brief Constructor for AutonomyCore
   * @param node Shared pointer to the ROS2 node
   */
  explicit AutonomyCore(rclcpp::Node::SharedPtr node);

  /**
   * @brief Destructor for AutonomyCore
   */
  ~AutonomyCore() = default;

  /**
   * @brief Initialize the autonomy core
   * @return True if initialization was successful
   */
  bool initialize();

  /**
   * @brief Load a behavior tree from an XML file
   * @param xml_file Path to the XML file
   * @return The loaded tree
   * @throws std::exception if loading fails
   */
  BT::Tree load_tree(const std::string & xml_file);

  /**
   * @brief Get the BehaviorTree factory
   * @return Reference to the BehaviorTree factory
   */
  BT::BehaviorTreeFactory & get_factory()
  {
    return factory_;
  }

public:
  /**
   * @brief Register a custom node type with the factory using a builder lambda
   * @param node_type The node type name (ID for registration)
   * @param builder The builder function for the node type
   */
  template <typename NodeType>
  void register_custom_node(
    const std::string & node_type,
    const BT::NodeBuilder & builder)
  {
    try {
      factory_.registerBuilder<NodeType>(node_type, builder);
      RCLCPP_DEBUG(node_->get_logger(), "Registered custom node type: %s", node_type.c_str());
    } catch (const std::exception & e) {
      RCLCPP_ERROR(
        node_->get_logger(), "Failed to register custom node %s: %s", node_type.c_str(),
        e.what());
    }
  }

protected:
  rclcpp::Node::SharedPtr node_;
  BT::BehaviorTreeFactory factory_;
};

}  // namespace mercury_autonomy

#endif  // MERCURY_AUTONOMY__AUTONOMY_CORE_H_
