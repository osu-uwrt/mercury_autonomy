#ifndef MERCURY_AUTONOMY__NODES__EXAMPLE_ACTION_NODE_H_
#define MERCURY_AUTONOMY__NODES__EXAMPLE_ACTION_NODE_H_

/**
 * @file example_action_node.h
 * @brief Example custom action node for developers to use as a template
 *
 * This file demonstrates how to create a custom action node that inherits from
 * ROS2ActionNode. Copy and modify this file to create new action nodes.
 *
 * Steps to create a new action node:
 * 1. Copy this file and rename it (e.g., my_custom_action.h)
 * 2. Update the CLASS_NAME and class name
 * 3. Implement the onStart() and onRunning() methods
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
 * @class ExampleActionNode
 * @brief Example action node demonstrating common patterns
 *
 * This node serves as a template for creating custom action nodes.
 * It demonstrates:
 * - Accessing the ROS2 node through get_node()
 * - Handling input/output ports
 * - State management between ticks
 * - Logging with ROS2 logger
 */
class ExampleActionNode : public ROS2ActionNode
{
public:
  /**
   * @brief Constructor
   * @param name The name of the node
   * @param config The node configuration from the BehaviorTree
   * @param node The ROS2 node for communication
   */
  explicit ExampleActionNode(
    const std::string & name,
    const BT::NodeConfig & config,
    rclcpp::Node::SharedPtr node)
  : ROS2ActionNode(name, config, node)
  {
    RCLCPP_DEBUG(get_node()->get_logger(), "ExampleActionNode created");
  }

  /**
   * @brief Destructor
   */
  ~ExampleActionNode() override = default;

  /**
   * @brief Provide input/output ports for the BehaviorTree
   * 
   * Ports are how nodes communicate with each other and the tree.
   * Each port has a type and an optional default value.
   * 
   * @return PortsList containing input and output port definitions
   *
   * Example:
   * @code
   * return PortsList{
   *   InputPort<std::string>("target_zone", "Zone to move to"),
   *   OutputPort<bool>("success", "Whether the action succeeded")
   * };
   * @endcode
   */
  static BT::PortsList providedPorts()
  {
    return BT::PortsList{};
  }

protected:
  // Add custom member variables here as needed
  bool initialized_{false};

  /**
   * @brief Called when the action starts
   * 
   * This method is called when the node transitions from IDLE to RUNNING.
   * Use this to initialize any resources or publish commands.
   * 
   * Return values:
   * - SUCCESS: Action completed immediately
   * - FAILURE: Action failed immediately
   * - RUNNING: Action needs more time (onRunning will be called next tick)
   * 
   * @return The status after starting
   */
  BT::NodeStatus onStart() override
  {
    RCLCPP_INFO(get_node()->get_logger(), "ExampleActionNode started");

    // TODO: Add your startup logic here
    // Example:
    //   - Read input ports with getInput<Type>("port_name")
    //   - Initialize resources
    //   - Publish requests to other ROS2 nodes
    //   - Set up callbacks or subscriptions

    initialized_ = true;

    // For immediate completion:
    // return BT::NodeStatus::SUCCESS;

    // For asynchronous work:
    return BT::NodeStatus::RUNNING;
  }

  /**
   * @brief Called while the action is running
   * 
   * This method is called every tree tick while the node status is RUNNING.
   * Use this to check if asynchronous work has completed.
   * 
   * Return values:
   * - SUCCESS: Action completed successfully
   * - FAILURE: Action failed
   * - RUNNING: Action still in progress
   * 
   * @return The status after this update
   */
  BT::NodeStatus onRunning() override
  {
    RCLCPP_DEBUG(get_node()->get_logger(), "ExampleActionNode running");

    // TODO: Add your execution logic here
    // Example:
    //   - Check if asynchronous work completed
    //   - Monitor feedback from other ROS2 nodes
    //   - Update output ports with results
    //   - Handle timeouts or errors

    // For now, always succeed:
    return BT::NodeStatus::SUCCESS;

    // Return RUNNING to continue waiting:
    // return BT::NodeStatus::RUNNING;

    // Return FAILURE if something went wrong:
    // return BT::NodeStatus::FAILURE;
  }

  /**
   * @brief Called when the node is halted
   * 
   * This method is called when the node is stopped, usually because:
   * - Its parent sequence/selector was canceled
   * - The tree execution was stopped
   * - A sibling node caused the tree to stop visiting this node
   * 
   * Use this to clean up resources:
   * - Cancel pending ROS2 requests
   * - Unsubscribe from topics
   * - Stop publishing commands
   * - Release memory or other resources
   */
  void onHalted() override
  {
    RCLCPP_INFO(get_node()->get_logger(), "ExampleActionNode halted");

    // TODO: Add cleanup logic here
    // Example:
    //   - Cancel any pending goals
    //   - Publish stop commands
    //   - Close connections

    initialized_ = false;
  }
};

}  // namespace mercury_autonomy::nodes

#endif  // MERCURY_AUTONOMY__NODES__EXAMPLE_ACTION_NODE_H_
