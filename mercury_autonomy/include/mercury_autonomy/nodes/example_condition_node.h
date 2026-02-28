#ifndef MERCURY_AUTONOMY__NODES__EXAMPLE_CONDITION_NODE_H_
#define MERCURY_AUTONOMY__NODES__EXAMPLE_CONDITION_NODE_H_

/**
 * @file example_condition_node.h
 * @brief Example custom condition node for developers to use as a template
 *
 * This file demonstrates how to create a custom condition node that inherits from
 * ROS2ConditionNode. Copy and modify this file to create new condition nodes.
 *
 * Steps to create a new condition node:
 * 1. Copy this file and rename it (e.g., my_custom_condition.h)
 * 2. Update the CLASS_NAME and class name
 * 3. Implement the tick() method with your condition logic
 * 4. Add any custom member variables needed
 * 5. Register the node in your tree loading code (see action_server.cpp)
 */

#include <string>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include "mercury_autonomy/nodes/custom_node_base.h"

namespace mercury_autonomy::nodes
{

/**
 * @class ExampleConditionNode
 * @brief Example condition node demonstrating how to check conditions
 *
 * Condition nodes are typically fast checks that don't perform actions.
 * They should return SUCCESS or FAILURE immediately without returning RUNNING.
 * They are used to make decisions in the behavior tree.
 *
 * Common use cases:
 * - Check if a sensor reading is within bounds
 * - Check if a flag or state variable is set
 * - Check if a ROS2 parameter has a certain value
 * - Verify that prerequisites are met before executing actions
 */
class ExampleConditionNode : public ROS2ConditionNode
{
public:
  /**
   * @brief Constructor
   * @param name The name of the node
   * @param config The node configuration from the BehaviorTree
   * @param node The ROS2 node for communication
   */
  explicit ExampleConditionNode(
    const std::string & name,
    const BT::NodeConfig & config,
    rclcpp::Node::SharedPtr node)
  : ROS2ConditionNode(name, config, node)
  {
    RCLCPP_DEBUG(get_node()->get_logger(), "ExampleConditionNode created");
  }

  /**
   * @brief Destructor
   */
  ~ExampleConditionNode() override = default;

  /**
   * @brief Provide input/output ports for the BehaviorTree
   * 
   * Condition nodes typically only have input ports (reading data)
   * and no output ports (they just return SUCCESS or FAILURE).
   * 
   * @return PortsList containing port definitions
   *
   * Example:
   * @code
   * return PortsList{
   *   InputPort<double>("battery_level", "Battery level percentage"),
   *   InputPort<double>("min_voltage", "Minimum acceptable voltage")
   * };
   * @endcode
   */
  static BT::PortsList providedPorts()
  {
    return BT::PortsList{};
  }

protected:
  /**
   * @brief Evaluate the condition
   * 
   * This method is called every tree tick when this node is visited.
   * It should evaluate the condition and return immediately without
   * performing any long-running operations.
   * 
   * Return values:
   * - SUCCESS: The condition is satisfied
   * - FAILURE: The condition is not satisfied
   * - RUNNING: Should NOT be used for condition nodes
   * 
   * @return SUCCESS if the condition is met, FAILURE otherwise
   */
  BT::NodeStatus tick() override
  {
    RCLCPP_DEBUG(get_node()->get_logger(), "ExampleConditionNode tick");

    // TODO: Implement your condition check here
    // Example condition checks:
    //   1. Read a parameter: auto value = get_node()->get_parameter("param_name")
    //   2. Access a shared data struct from input port
    //   3. Check sensor data or robot state
    //   4. Verify prerequisites with service calls (but keep it fast!)

    // Examples of different returns:

    // Always succeed:
    // return BT::NodeStatus::SUCCESS;

    // Always fail:
    // return BT::NodeStatus::FAILURE;

    // Conditional based on some logic:
    if (true) {  // Replace with your actual condition
      RCLCPP_DEBUG(get_node()->get_logger(), "Condition satisfied");
      return BT::NodeStatus::SUCCESS;
    } else {
      RCLCPP_DEBUG(get_node()->get_logger(), "Condition not satisfied");
      return BT::NodeStatus::FAILURE;
    }
  }
};

}  // namespace mercury_autonomy::nodes

#endif  // MERCURY_AUTONOMY__NODES__EXAMPLE_CONDITION_NODE_H_
