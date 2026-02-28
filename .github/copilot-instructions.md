# mercury_autonomy -- Copilot Instructions

## Project Context

This repository contains the `mercury_autonomy` package, a BehaviorTree-based
autonomy system for OSU UWRT's Mercury robot stack. It is the successor to
riptide_autonomy (https://github.com/osu-uwrt/riptide_autonomy), rebuilt around
BehaviorTree.CPP v4 with significant architectural differences.

The defining feature of mercury_autonomy is that the BehaviorTree.CPP library
(https://github.com/BehaviorTree/BehaviorTree.CPP) is vendored as a Git subtree
under `dependencies/BehaviorTreeCPP/`. This allows the team to modify the library
directly for additional features without maintaining a separate fork.

There is no backwards compatibility requirement with riptide_autonomy or any older code.

## Repository Structure

The repository root is the Git repository containing:

- `dependencies/BehaviorTreeCPP/` -- BT.CPP v4.9.0 as a Git subtree. This is a
  standalone ament_cmake package named `behaviortree_cpp`. Do not modify files here
  unless intentionally patching the library.
- `mercury_autonomy/` -- The main ROS2 ament_cmake package.
- `Project.md` -- Developer documentation.

### mercury_autonomy Package Layout

```
mercury_autonomy/
  CMakeLists.txt             -- Build configuration
  package.xml                -- ROS2 package manifest
  include/mercury_autonomy/
    autonomy_base.hpp        -- Common includes (ROS2, BT.CPP, TF2, msgs)
    autonomy_lib.hpp         -- Aggregate convenience header
    autonomy_util.hpp        -- Utility function declarations
    mercury_bt_node.hpp      -- Base classes: MercuryBtNode, MercuryActionNode,
                                MercuryConditionNode, MercuryDecoratorNode
    bt_actions/              -- Headers for custom BT action nodes
    bt_conditions/           -- Headers for custom BT condition nodes
    bt_decorators/           -- Headers for custom BT decorator nodes
  src/
    autonomy_util.cpp        -- Plugin registration, ROS-BT init, geometry helpers
    mercury_bt_node.cpp      -- MercuryBtNode (TF2 + ROS bridge) implementation
    tree_executor.cpp        -- ROS2 node with action server to run BT XML files
    register_actions.cpp     -- BT_REGISTER_NODES for action plugin library
    register_conditions.cpp  -- BT_REGISTER_NODES for condition plugin library
    register_decorators.cpp  -- BT_REGISTER_NODES for decorator plugin library
    bt_actions/              -- Action node .cpp implementations
    bt_conditions/           -- Condition node .cpp implementations
    bt_decorators/           -- Decorator node .cpp implementations
  trees/                     -- BT XML files (loaded by tree_executor at runtime)
```

## Build System

- Both packages use `ament_cmake` and are built with `colcon build`.
- mercury_autonomy depends on `behaviortree_cpp` (declared in package.xml).
- colcon automatically discovers both packages and builds them in dependency order.
- Custom BT nodes are compiled into three shared libraries (actions, conditions,
  decorators) loaded as BT.CPP plugins at runtime via `BT_REGISTER_NODES`.
- New source files in `src/bt_actions/`, `src/bt_conditions/`, `src/bt_decorators/`
  are picked up automatically by CMake `file(GLOB ...)`. Only the corresponding
  `register_*.cpp` file needs a manual edit to register a new node type.

### Build Command

```bash
colcon build --packages-select behaviortree_cpp mercury_autonomy \
    --allow-overriding behaviortree_cpp \
    --cmake-args -DBTCPP_EXAMPLES=OFF -DBUILD_TESTING=OFF -DBTCPP_BUILD_TOOLS=OFF
```

## Key Architectural Patterns

### ROS-BT Bridge (MercuryBtNode)

Every custom BT node class inherits from both a BT.CPP base class and
`MercuryBtNode`. This provides:
- A shared `rclcpp::Node` handle accessible via `rosNode()`.
- A shared TF2 buffer/listener for transform lookups.
- A `rosInit()` virtual method called once before the first tick, used to create
  publishers, subscribers, service clients, etc.

Three concrete base classes exist:
- `MercuryActionNode` (extends `BT::StatefulActionNode`) -- multi-tick actions.
- `MercuryConditionNode` (extends `BT::ConditionNode`) -- synchronous predicates.
- `MercuryDecoratorNode` (extends `BT::DecoratorNode`) -- child wrappers.

### Tree Executor

`tree_executor` is the main executable. It:
1. Registers all plugin libraries from the ament install path.
2. Provides an `autonomy/list_trees` service.
3. Loads and ticks tree XML files via `executeTree(path)`.
4. Publishes status on `autonomy/status`.

A formal ROS2 action interface (with goal/feedback/result) should be added once
project-specific action message types are defined.

### Plugin Registration

New nodes are registered by adding a `factory.registerNodeType<>()` call in the
appropriate `src/register_<type>.cpp` file. The `BT_REGISTER_NODES` macro exports
the registration function from the shared library so BT.CPP can load it at runtime.

## Coding Conventions

- C++17 standard, compiled with `-Wall -Wextra -Wpedantic`.
- All custom code lives in the `mercury_autonomy` namespace.
- Header files use `#pragma once`.
- Naming: snake_case for files, variables, and functions; PascalCase for classes
  and BT node type names.
- Each custom node has a header in `include/mercury_autonomy/bt_<type>/` and a
  corresponding implementation in `src/bt_<type>/`.
- Concise comments are preferred -- describe *why*, not *what*.

## Environment

| Item       | Value              |
|------------|--------------------|
| OS         | Ubuntu 22.04       |
| ROS Distro | ROS2 Humble        |
| BT.CPP     | v4.9.0 (subtree)   |
| C++        | C++17              |

## Common Tasks for Agents

### Adding a new BT action node

1. Create header: `include/mercury_autonomy/bt_actions/<name>.hpp`
   - Inherit from `MercuryActionNode`.
   - Implement `providedPorts()`, `onStart()`, `onRunning()`, `onHalted()`, `rosInit()`.
2. Create implementation: `src/bt_actions/<name>.cpp`
3. Register: add `factory.registerNodeType<...>("NodeName")` in `src/register_actions.cpp`.
4. Rebuild.

### Adding a new BT condition node

Same pattern as actions, using `MercuryConditionNode`, `tick()`, and
`src/register_conditions.cpp`.

### Adding a new BT decorator node

Same pattern as actions, using `MercuryDecoratorNode`, `tick()`, and
`src/register_decorators.cpp`.

### Modifying the BT.CPP library

Edit files directly under `dependencies/BehaviorTreeCPP/`. Commit normally.
To pull upstream changes:

```bash
git subtree pull --prefix=dependencies/BehaviorTreeCPP btCPP_upstream master --squash
```

### Adding a new ROS2 dependency

1. Add `find_package(<dep> REQUIRED)` in CMakeLists.txt.
2. Add `<dep>` to the `THIS_PACKAGE_DEPS` list in CMakeLists.txt.
3. Add `<depend><dep></depend>` in package.xml.
