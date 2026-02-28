# mercury_autonomy

BehaviorTree-based autonomy system for the Mercury robot stack, built on
[BT.CPP v4](https://github.com/BehaviorTree/BehaviorTree.CPP) with the library
vendored as a Git subtree under `dependencies/BehaviorTreeCPP/`.

See [Project.md](../Project.md) for full developer documentation.

## Quick Start

### Building

From the colcon workspace root:

```bash
colcon build --packages-select behaviortree_cpp mercury_autonomy \
    --allow-overriding behaviortree_cpp \
    --cmake-args -DBTCPP_EXAMPLES=OFF -DBUILD_TESTING=OFF -DBTCPP_BUILD_TOOLS=OFF
```

### Running

```bash
source install/setup.bash
ros2 launch mercury_autonomy autonomy.launch.py
# Or with a namespace:
ros2 launch mercury_autonomy autonomy.launch.py robot:=mercury
```

### Running Tests

```bash
colcon build --packages-select mercury_autonomy \
    --cmake-args -DBUILD_TESTING=ON -DBTCPP_EXAMPLES=OFF -DBTCPP_BUILD_TOOLS=OFF
colcon test --packages-select mercury_autonomy
colcon test-result --verbose
```

## BT Node Assistant

The `scripts/bt_assistant.py` tool automates creation of new BT nodes. It
generates the header, source, and registration entry from templates.

### Creating a new node

```bash
cd mercury_autonomy/                   # Package root (contains CMakeLists.txt)
python3 scripts/bt_assistant.py create action MoveToWaypoint
python3 scripts/bt_assistant.py create condition IsSubmerged
python3 scripts/bt_assistant.py create decorator RetryOnFail
```

This creates:
- `include/mercury_autonomy/bt_actions/move_to_waypoint.hpp`
- `src/bt_actions/move_to_waypoint.cpp`
- Registration line in `src/register_actions.cpp`

### Listing existing nodes

```bash
python3 scripts/bt_assistant.py list
```

### Checking consistency

Verifies that all source files have matching headers and registration entries:

```bash
python3 scripts/bt_assistant.py check
```

## Package Structure

```
mercury_autonomy/
  CMakeLists.txt
  package.xml
  launch/                    -- ROS2 launch files
  scripts/                   -- Development tools (bt_assistant)
  test/                      -- GTest-based unit and integration tests
  trees/                     -- BT XML files loaded at runtime
  include/mercury_autonomy/
    autonomy_base.hpp        -- Common includes
    autonomy_lib.hpp         -- Convenience aggregate header
    autonomy_util.hpp        -- Utility functions
    mercury_bt_node.hpp      -- MercuryBtNode base classes
    bt_actions/              -- Action node headers
    bt_conditions/           -- Condition node headers
    bt_decorators/           -- Decorator node headers
  src/
    autonomy_util.cpp        -- Plugin registration, geometry helpers
    mercury_bt_node.cpp      -- ROS-BT bridge implementation
    tree_executor.cpp        -- Main ROS2 node (runs BT trees)
    register_actions.cpp     -- Action plugin registration
    register_conditions.cpp  -- Condition plugin registration
    register_decorators.cpp  -- Decorator plugin registration
    bt_actions/              -- Action implementations
    bt_conditions/           -- Condition implementations
    bt_decorators/           -- Decorator implementations
```
