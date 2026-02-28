#ifndef MERCURY_AUTONOMY__NODES__CUSTOM_NODE_BASE_H_
#define MERCURY_AUTONOMY__NODES__CUSTOM_NODE_BASE_H_

#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <behaviortree_cpp/action_node.h>
#include <behaviortree_cpp/condition_node.h>

namespace mercury_autonomy::nodes
{

/**
 * @class ROS2ActionNode
 * @brief Base class for ROS2-aware action nodes in the behavior tree
 *
 * This class provides a foundation for creating custom action nodes that need
 * access to a ROS2 node for communication.
 *
 * Usage:
 * @code
 * class MyCustomAction : public ROS2ActionNode
 * {
 * public:
 *   explicit MyCustomAction(
 *     const std::string& name,
 *     const BT::NodeConfig& config,
 *     rclcpp::Node::SharedPtr node)
 *     : ROS2ActionNode(name, config, node) {}
 *
 * private:
 *   BT::NodeStatus onStart() override {
 *     // Called when the action starts
 *     return BT::NodeStatus::RUNNING;
 *   }
 *
 *   BT::NodeStatus onRunning() override {
 *     // Called while the action is running
 *     return BT::NodeStatus::RUNNING;
 *   }
 *
 *   void onHalted() override {
 *     // Called when the action is stopped
 *   }
 * };
 * @endcode
 */
class ROS2ActionNode : public BT::AsyncActionNode
{
public:
  /**
   * @brief Constructor
   * @param name The name of the node
   * @param config The node configuration
   * @param node The ROS2 node for communication
   */
  ROS2ActionNode(
    const std::string & name,
    const BT::NodeConfig & config,
    rclcpp::Node::SharedPtr node)
  : BT::AsyncActionNode(name, config), node_(node)
  {
  }

  /**
   * @brief Get the ROS2 node
   * @return Shared pointer to the ROS2 node
   */
  rclcpp::Node::SharedPtr get_node() { return node_; }

protected:
  rclcpp::Node::SharedPtr node_;

  /**
   * @brief Called when the action starts
   * @return RUNNING if the action needs more processing, SUCCESS/FAILURE if complete
   */
  virtual BT::NodeStatus onStart() = 0;

  /**
   * @brief Called while the action is running
   * @return RUNNING to continue, SUCCESS/FAILURE when complete
   */
  virtual BT::NodeStatus onRunning()
  {
    // Default implementation: do nothing, return SUCCESS
    return BT::NodeStatus::SUCCESS;
  }

  /**
   * @brief Called when the action is halted
   */
  virtual void onHalted() {}

  /**
   * @brief BehaviorTree execution tick method
   * @return The status after this tick
   */
  BT::NodeStatus tick() override
  {
    return onStart();
  }
};

/**
 * @class ROS2ConditionNode
 * @brief Base class for ROS2-aware condition nodes in the behavior tree
 *
 * This class provides a foundation for creating custom condition nodes that need
 * access to a ROS2 node for communication.
 *
 * Usage:
 * @code
 * class MyCustomCondition : public ROS2ConditionNode
 * {
 * public:
 *   explicit MyCustomCondition(
 *     const std::string& name,
 *     const BT::NodeConfig& config,
 *     rclcpp::Node::SharedPtr node)
 *     : ROS2ConditionNode(name, config, node) {}
 *
 * private:
 *   BT::NodeStatus tick() override {
 *     // Check your condition here
 *     if (condition_met) {
 *       return BT::NodeStatus::SUCCESS;
 *     } else {
 *       return BT::NodeStatus::FAILURE;
 *     }
 *   }
 * };
 * @endcode
 */
class ROS2ConditionNode : public BT::ConditionNode
{
public:
  /**
   * @brief Constructor
   * @param name The name of the node
   * @param config The node configuration
   * @param node The ROS2 node for communication
   */
  ROS2ConditionNode(
    const std::string & name,
    const BT::NodeConfig & config,
    rclcpp::Node::SharedPtr node)
  : BT::ConditionNode(name, config), node_(node)
  {
  }

  /**
   * @brief Get the ROS2 node
   * @return Shared pointer to the ROS2 node
   */
  rclcpp::Node::SharedPtr get_node() { return node_; }

protected:
  rclcpp::Node::SharedPtr node_;
};

}  // namespace mercury_autonomy::nodes

#endif  // MERCURY_AUTONOMY__NODES__CUSTOM_NODE_BASE_H_
