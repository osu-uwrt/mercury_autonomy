#ifndef MERCURY_AUTONOMY__NODES__CREATING_CUSTOM_NODES_H_
#define MERCURY_AUTONOMY__NODES__CREATING_CUSTOM_NODES_H_

/**
 * @file creating_custom_nodes.h
 * @brief Comprehensive guide for creating custom BehaviorTree nodes in mercury_autonomy
 *
 * This document provides detailed instructions on how to create and register
 * custom BehaviorTree nodes in the mercury_autonomy package.
 *
 * QUICK START
 * ===========
 * 1. Copy include/mercury_autonomy/nodes/example_action_node.h (or _condition_node.h)
 * 2. Rename and modify it with your logic
 * 3. Add your node files to src/CMakeLists.txt if they have implementation
 * 4. Register in action_server.cpp using autonomy_core_->register_custom_node()
 *
 * FILE STRUCTURE
 * ==============
 * Header-only nodes:
 *   - include/mercury_autonomy/nodes/my_custom_node.h
 *   - No changes to CMakeLists.txt needed
 *   - Register: autonomy_core_->register_custom_node("MyCustomNode", ...)
 *
 * With implementation file:
 *   - include/mercury_autonomy/nodes/my_custom_node.h
 *   - src/my_custom_node.cpp
 *   - Update CMakeLists.txt to include in library compilation
 *
 * TYPES OF NODES
 * ==============
 *
 * 1. ACTION NODES (inherits ROS2ActionNode)
 *    Purpose: Perform actions that may take multiple ticks
 *    Returns: RUNNING while executing, SUCCESS/FAILURE when done
 *    Use for: Motion commands, waiting for responses, complex tasks
 *    Example: See example_action_node.h
 *
 * 2. CONDITION NODES (inherits ROS2ConditionNode)
 *    Purpose: Check conditions instantly
 *    Returns: SUCCESS or FAILURE immediately (never RUNNING)
 *    Use for: Checks, guards, branches in decision logic
 *    Example: See example_condition_node.h
 *
 * 3. DECORATOR NODES (inherits BT::DecoratorNode)
 *    Purpose: Modify behavior of child nodes
 *    Common examples: Repeat, Retry, Timeout, Invert
 *    See BehaviorTree.CPP documentation for details
 *
 * 4. CONTROL NODES (inherits BT::ControlNode)
 *    Purpose: Control flow - Sequence, Selector, Parallel
 *    Usually built-in, create custom ones only if needed
 *    See BehaviorTree.CPP documentation for details
 *
 * REGISTERING NODES
 * =================
 *
 * In action_server.cpp, add your node registration in initialize():
 *
 * @code
 * using CreateNodeFunc = std::function<BT::Node::Ptr(
 *   const std::string&, const BT::NodeConfig&, rclcpp::Node::SharedPtr)>;
 *
 * // For action nodes:
 * auto my_action_builder = [this](
 *   const std::string& name,
 *   const BT::NodeConfig& config) -> BT::Node::Ptr {
 *   return std::make_unique<MyCustomAction>(name, config, shared_from_this());
 * };
 *
 * autonomy_core_->register_custom_node("MyCustomAction", my_action_builder);
 * @endcode
 *
 * NODE LIFECYCLE
 * ==============
 *
 * For ACTION NODES:
 *   1. Node created (constructor called)
 *   2. onStart() called when node first ticked
 *   3. If RUNNING: onRunning() called each subsequent tick
 *   4. If SUCCESS/FAILURE: node complete for this tree execution
 *   5. onHalted() called when node is stopped/cancelled
 *
 * For CONDITION NODES:
 *   1. Node created (constructor called)
 *   2. tick() called when node is visited
 *   3. Immediately returns SUCCESS or FAILURE
 *   4. No onHalted() needed (no cleanup for instant checks)
 *
 * PORTS AND DATA FLOW
 * ===================
 *
 * Ports allow nodes to communicate with each other through the blackboard.
 * 
 * Input ports: Read data from the blackboard
 * Output ports: Write data to the blackboard
 * 
 * Example with ports:
 * @code
 * class MoveRobotNode : public ROS2ActionNode {
 * public:
 *   static BT::PortsList providedPorts() {
 *     return BT::PortsList{
 *       BT::InputPort<std::string>("target", "Target location"),
 *       BT::OutputPort<bool>("success", "Movement success")
 *     };
 *   }
 *   
 *   BT::NodeStatus onStart() override {
 *     auto target = getInput<std::string>("target");
 *     if (!target) {
 *       return BT::NodeStatus::FAILURE;
 *     }
 *     // Use target.value() to access the data
 *     return BT::NodeStatus::RUNNING;
 *   }
 *   
 *   BT::NodeStatus onRunning() override {
 *     // Check if movement complete
 *     if (movement_complete) {
 *       setOutput<bool>("success", true);
 *       return BT::NodeStatus::SUCCESS;
 *     }
 *     return BT::NodeStatus::RUNNING;
 *   }
 * };
 * @endcode
 *
 * BEST PRACTICES
 * ==============
 *
 * 1. Keep nodes focused on a single responsibility
 * 2. Use ROS2 logging (RCLCPP_DEBUG, RCLCPP_INFO, etc.) for debugging
 * 3. Handle onHalted() to cancel ongoing operations
 * 4. Avoid blocking operations in tick methods
 * 5. Use the blackboard for inter-node communication
 * 6. Document input/output ports clearly
 * 7. Test nodes independently before integrating into trees
 * 8. Use meaningful node names that describe their purpose
 * 9. Handle errors gracefully and log them appropriately
 * 10. Consider performance - keep ticks fast and non-blocking
 *
 * COMMON PATTERNS
 * ===============
 *
 * Pattern 1: Action with async callback
 * @code
 * BT::NodeStatus onStart() override {
 *   // Send async request
 *   future_ = client_->async_send_request(...);
 *   return BT::NodeStatus::RUNNING;
 * }
 *
 * BT::NodeStatus onRunning() override {
 *   // Check if response received
 *   if (future_.valid() && future_.wait_for(0s) == std::future_status::ready) {
 *     auto result = future_.get();
 *     return result.success ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
 *   }
 *   return BT::NodeStatus::RUNNING;
 * }
 * @endcode
 *
 * Pattern 2: Polling a sensor/service
 * @code
 * BT::NodeStatus onStart() override {
 *   start_time_ = get_node()->get_clock()->now();
 *   return BT::NodeStatus::RUNNING;
 * }
 *
 * BT::NodeStatus onRunning() override {
 *   auto current_value = read_sensor();
 *   if (is_target_reached(current_value)) {
 *     return BT::NodeStatus::SUCCESS;
 *   }
 *   if (is_timeout(start_time_)) {
 *     return BT::NodeStatus::FAILURE;
 *   }
 *   return BT::NodeStatus::RUNNING;
 * }
 * @endcode
 *
 * DEBUGGING TIPS
 * ==============
 *
 * 1. Enable debug logging: ros2 run mercury_autonomy autonomy_action_server --ros-args --log-level DEBUG
 * 2. Use the Groot2 interface to visualize tree execution (if available)
 * 3. Add logging at key points: onStart(), state changes, onRunning()
 * 4. Test with simple XML trees first before complex ones
 * 5. Check the ROS2 parameter server for any required parameters
 * 6. Monitor ROS2 topics and services for unexpected state changes
 *
 */

#endif  // MERCURY_AUTONOMY__NODES__CREATING_CUSTOM_NODES_H_
