# Mercury Autonomy Project Documentation

## Overview

**mercury_autonomy** is a BehaviorTree-based autonomy system for the Mercury stack. It replaces riptide_autonomy with a modern, flexible architecture powered by [BehaviorTree.CPP](https://github.com/BehaviorTree/BehaviorTree.CPP).

The defining feature of mercury_autonomy is that the BehaviorTree.CPP library is vendored as a Git subtree in the `dependencies/BehaviorTreeCPP/` directory, allowing custom modifications and rapid iteration on library features while maintaining the ability to pull upstream updates.

## Architecture

### Core Components

#### 1. **AutonomyCore** (`include/mercury_autonomy/autonomy_core.h`)
- Manages the BehaviorTree factory and tree loading
- Handles custom node registration
- Responsible for parsing XML tree definitions
- Provides the factory interface for creating trees

#### 2. **TreeManager** (`include/mercury_autonomy/tree_manager.h`)
- Manages tree execution lifecycle in a background thread
- Provides pause/resume functionality
- Maintains tree execution status
- Allows configurable tick rate

#### 3. **AutonomyActionServer** (`include/mercury_autonomy/action_server.h`)
- ROS2 action server interface for tree execution
- Coordinates AutonomyCore and TreeManager
- Integrates with the Mercury stack communication system
- Supports concurrent goal handling

### Custom Node Framework

The package provides a flexible framework for creating custom BehaviorTree nodes:

#### Base Classes
- **ROS2ActionNode**: Base for action nodes (long-running operations)
- **ROS2ConditionNode**: Base for condition nodes (instant checks)

Both base classes provide:
- Automatic ROS2 node access via `get_node()`
- Integration with the BehaviorTree.CPP execution model
- Lifecycle hooks (onStart, onRunning, onHalted)

#### Creating Custom Nodes

See `include/mercury_autonomy/nodes/CREATING_CUSTOM_NODES.md` for comprehensive guide.

Quick start:
1. Copy `include/mercury_autonomy/nodes/example_action_node.h` or `example_condition_node.h`
2. Modify the class name and implement your logic
3. Register the node in your tree loading code
4. Use in XML tree definitions

Example action node structure:
```cpp
class MyNode : public ROS2ActionNode {
  BT::NodeStatus onStart() override { /* Initialize */ }
  BT::NodeStatus onRunning() override { /* Update */ }
  void onHalted() override { /* Cleanup */ }
};
```

## Project Structure

```
mercury_autonomy/
├── include/mercury_autonomy/
│   ├── autonomy_core.h          # Core autonomy system
│   ├── tree_manager.h           # Tree execution management
│   ├── action_server.h          # ROS2 action server
│   └── nodes/
│       ├── custom_node_base.h   # Base classes for custom nodes
│       ├── example_action_node.h # Action node template
│       ├── example_condition_node.h # Condition node template
│       └── CREATING_CUSTOM_NODES.md # Custom node guide
├── src/
│   ├── autonomy_core.cpp        # Implementation
│   ├── tree_manager.cpp         # Thread-safe tree execution
│   ├── action_server.cpp        # Action server implementation
│   └── autonomy_action_server_main.cpp # Entry point
├── trees/
│   └── main.xml                 # Example tree definition
├── CMakeLists.txt               # Build configuration
├── package.xml                  # ROS2 package metadata
└── README.md                    # Quick reference
```

## Building

### Prerequisites
- ROS2 (tested on Iron/Jazzy)
- CMake 3.16+
- C++17 compiler

### Build Steps

```bash
cd /home/bobdobchob/osu-uwrt/development/software
colcon build --packages-select behaviortree_cpp mercury_autonomy
```

The build system automatically:
- Compiles the local BehaviorTree.CPP library
- Links mercury_autonomy against the local library
- Installs all headers and resources

### Build Targets
- `mercury_autonomy_lib` - Core library (shared)
- `autonomy_action_server` - Executable action server
- `autonomy_component_node` - ROS2 component node

## Usage

### Running the Action Server

```bash
ros2 run mercury_autonomy autonomy_action_server --ros-args --log-level INFO
```

Parameters:
- `tick_rate` (default: 10.0 Hz) - Tree execution tick rate

### Creating a Behavior Tree

XML Example (`trees/my_mission.xml`):
```xml
<?xml version="1.0" encoding="UTF-8"?>
<root main_tree_to_execute="MainTree">
  <BehaviorTree ID="MainTree">
    <Sequence name="mission">
      <Action ID="InitializeRobot" />
      <Condition ID="CheckMissionReady" />
      <Action ID="ExecuteMission" />
    </Sequence>
  </BehaviorTree>
</root>
```

### Registering Custom Nodes

In `src/action_server.cpp`, register your nodes:

```cpp
auto my_action = [this](
  const std::string& name,
  const BT::NodeConfig& config) -> BT::Node::Ptr {
  return std::make_unique<MyNode>(name, config, shared_from_this());
};

autonomy_core_->register_custom_node<MyNode>("MyNode", my_action);
```

## Dependencies

### Core Dependencies
- `rclcpp` - ROS2 C++ client library
- `rclcpp_action` - ROS2 action framework
- `rclcpp_components` - ROS2 component framework
- `behaviortree_cpp` - Local BehaviorTree.CPP subtree (dependencies/BehaviorTreeCPP)

### Transitive Dependencies (from BehaviorTree.CPP)
- `zeroMQ` - Messaging (for Groot2 interface)
- `TinyXML2` - XML parsing
- SQLite3 - Logging support

## Development Workflow

### Adding New Custom Nodes

1. Create header file in `include/mercury_autonomy/nodes/my_node.h`
2. Implement node logic following examples
3. Register in action server initialization
4. Add to XML tree definitions
5. Test and debug

### Debugging

Enable debug logging:
```bash
ros2 run mercury_autonomy autonomy_action_server \
  --ros-args --log-level mercury_autonomy:=DEBUG
```

Monitor execution:
```bash
ros2 topic echo /autonomy/node_status  # Custom status topic (to be implemented)
ros2 service list | grep autonomy    # List available services
```

### Testing Trees Locally

Create a test executable in `src/test_tree_main.cpp`:
```cpp
auto core = mercury_autonomy::AutonomyCore(node);
auto tree = core.load_tree("trees/main.xml");
auto manager = mercury_autonomy::TreeManager(node);
manager.load_tree(std::move(tree));
manager.start_execution();
```

## Dependency Management: BehaviorTree.CPP Subtree

The BehaviorTree.CPP library is managed as a Git subtree for customization while maintaining upstream sync capability.

### Working with the Subtree

#### Setup (One-time)
```bash
git remote add btCPP_upstream https://github.com/BehaviorTree/BehaviorTree.CPP.git
```

#### Pull Upstream Updates
```bash
git fetch btCPP_upstream master
git subtree pull --prefix=dependencies/BehaviorTreeCPP btCPP_upstream master --squash
```

#### Push Local Modifications
```bash
git subtree push --prefix=dependencies/BehaviorTreeCPP <your-fork> <branch>
```

#### Local Development
- Edit files in `dependencies/BehaviorTreeCPP/` directly
- Changes are tracked as part of mercury_autonomy repository
- Rebuild with `colcon build` to test changes

See [README.md](./README.md) for more details on subtree management.

## API Reference

### AutonomyCore

```cpp
class AutonomyCore {
  bool initialize();
  BT::Tree load_tree(const std::string& xml_file);
  
  template <typename T>
  void register_custom_node(const std::string& id, const BT::NodeBuilder& builder);
  
  BT::BehaviorTreeFactory& get_factory();
};
```

### TreeManager

```cpp
class TreeManager {
  TreeManager(rclcpp::Node::SharedPtr node, double tick_rate = 10.0);
  
  bool load_tree(BT::Tree&& tree);
  bool start_execution();
  bool stop_execution();
  
  BT::NodeStatus get_tree_status() const;
  bool is_running() const;
  
  void request_pause();
  void resume_execution();
  
  double get_tick_rate() const;
  void set_tick_rate(double tick_rate);
};
```

### Custom Node Bases

```cpp
class ROS2ActionNode : public BT::AsyncActionNode {
  rclcpp::Node::SharedPtr get_node();
  
  virtual BT::NodeStatus onStart() = 0;
  virtual BT::NodeStatus onRunning();
  virtual void onHalted();
};

class ROS2ConditionNode : public BT::ConditionNode {
  rclcpp::Node::SharedPtr get_node();
  
  virtual BT::NodeStatus tick() = 0;
};
```

## Future Work

- [ ] Complete action server goal/result handling
- [ ] Define standard action message types
- [ ] Implement node status feedback topics
- [ ] Add Groot2 integration for tree visualization
- [ ] Create standard mission library
- [ ] Performance optimization for high-frequency trees
- [ ] Test on actual Mercury robot hardware

## References

- [BehaviorTree.CPP Documentation](https://www.behaviortree.dev/)
- [riptide_autonomy](https://github.com/osu-uwrt/riptide_autonomy) - Previous implementation
- [ROS2 Action Framework](https://docs.ros.org/en/iron/Concepts/Intermediate/About-Actions.html)

## Contact

For questions or issues, contact: Aditya Gupta (gupta.1516@osu.edu)
