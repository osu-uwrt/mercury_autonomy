# mercury_autonomy -- Developer Documentation

## Overview

mercury_autonomy is a BehaviorTree-based autonomy package for the Mercury robot
stack. It serves as the successor to
[riptide_autonomy](https://github.com/osu-uwrt/riptide_autonomy), rebuilt around
[BehaviorTree.CPP v4](https://github.com/BehaviorTree/BehaviorTree.CPP) with the
library vendored as a Git subtree so the team can patch or extend it directly.

## Repository Layout

```
mercury_autonomy/              <-- Git repository root
  dependencies/
    BehaviorTreeCPP/           <-- BT.CPP v4 (Git subtree, ament_cmake package)
  mercury_autonomy/            <-- Main ROS2 package (ament_cmake)
    CMakeLists.txt
    package.xml
    include/mercury_autonomy/
      autonomy_base.hpp        <-- Common includes (ROS2, BT.CPP, TF2, messages)
      autonomy_lib.hpp         <-- Aggregate convenience header
      autonomy_util.hpp        <-- Utility function declarations
      mercury_bt_node.hpp      <-- MercuryBtNode base classes
      bt_actions/              <-- Custom action node headers
      bt_conditions/           <-- Custom condition node headers
      bt_decorators/           <-- Custom decorator node headers
    src/
      autonomy_util.cpp        <-- Utility function implementations
      mercury_bt_node.cpp      <-- MercuryBtNode implementation
      tree_executor.cpp        <-- Action server / tree runner
      register_actions.cpp     <-- Plugin registration for actions
      register_conditions.cpp  <-- Plugin registration for conditions
      register_decorators.cpp  <-- Plugin registration for decorators
      bt_actions/              <-- Action node implementations
      bt_conditions/           <-- Condition node implementations
      bt_decorators/           <-- Decorator node implementations
    trees/                     <-- BT XML files run by tree_executor
```

## Build System

The repository contains two colcon/ament_cmake packages:

| Package             | Location                          | Description                    |
|---------------------|-----------------------------------|--------------------------------|
| behaviortree_cpp    | dependencies/BehaviorTreeCPP/     | BT.CPP v4 library (subtree)   |
| mercury_autonomy    | mercury_autonomy/                 | Main autonomy package          |

colcon discovers both automatically. mercury_autonomy declares a `<depend>` on
`behaviortree_cpp` in its `package.xml` so colcon builds the library first.

### Building

From the workspace root (the directory containing `src/`):

```bash
colcon build --packages-select behaviortree_cpp mercury_autonomy \
    --allow-overriding behaviortree_cpp \
    --cmake-args -DBTCPP_EXAMPLES=OFF -DBUILD_TESTING=OFF -DBTCPP_BUILD_TOOLS=OFF
```

The `--allow-overriding` flag is needed because behaviortree_cpp may also be
installed system-wide or in another workspace.

### Build Artifacts

| Artifact                        | Type              | Purpose                              |
|---------------------------------|-------------------|--------------------------------------|
| libmercury_autonomy_core.so    | Shared library    | Core utilities and ROS-BT bridge     |
| libmercury_bt_actions.so       | BT plugin library | All custom action nodes              |
| libmercury_bt_conditions.so    | BT plugin library | All custom condition nodes           |
| libmercury_bt_decorators.so    | BT plugin library | All custom decorator nodes           |
| tree_executor                   | Executable        | ROS2 node that runs BT XML files     |

## Architecture

### MercuryBtNode (ROS-BT Bridge)

Every custom node inherits from both a BT.CPP base class and `MercuryBtNode`.
This dual inheritance gives each node:

- Access to a shared `rclcpp::Node` handle (`rosNode()`)
- A shared TF2 buffer for transform lookups
- A `rosInit()` hook called once before the first tick

Three concrete base classes are provided:

- `MercuryActionNode` -- for stateful, multi-tick actions
- `MercuryConditionNode` -- for synchronous boolean checks
- `MercuryDecoratorNode` -- for wrapping a single child

### Tree Executor

`tree_executor` is a ROS2 node that:

1. Registers all plugin libraries with the BT factory at startup.
2. Provides an `autonomy/list_trees` service to enumerate available XML files.
3. Has an `executeTree(path)` method that loads a tree, initializes ROS
   integration, and ticks it until completion.
4. Publishes status updates on `autonomy/status`.

A proper ROS2 action interface (goal/feedback/result) should be added once a
project-specific action message type is defined.

### Plugin Registration

Each node type has a shared library that exports a `BT_REGISTER_NODES` function.
New nodes are added by:

1. Writing the header in `include/mercury_autonomy/bt_<type>/`
2. Writing the implementation in `src/bt_<type>/`
3. Adding a `registerNodeType` call in `src/register_<type>.cpp`

The CMakeLists.txt uses `file(GLOB ...)` to pick up source files automatically,
so only the registration file needs a manual edit.

## Creating a New Custom Node

### Action Node

1. Copy `include/mercury_autonomy/bt_actions/example_action.hpp` to a new file.
2. Copy `src/bt_actions/example_action.cpp` to a new file.
3. Rename the class, update `providedPorts()`, and implement the callbacks.
4. Add a `factory.registerNodeType<>()` call in `src/register_actions.cpp`.

### Condition Node

1. Copy `include/mercury_autonomy/bt_conditions/example_condition.hpp`.
2. Copy `src/bt_conditions/example_condition.cpp`.
3. Rename, update ports and `tick()`.
4. Register in `src/register_conditions.cpp`.

### Decorator Node

1. Copy `include/mercury_autonomy/bt_decorators/example_decorator.hpp`.
2. Copy `src/bt_decorators/example_decorator.cpp`.
3. Rename, update ports and `tick()`.
4. Register in `src/register_decorators.cpp`.

## BT.CPP Subtree Management

The BehaviorTree.CPP library is included as a Git subtree under
`dependencies/BehaviorTreeCPP/`. It tracks the upstream `master` branch.

### Pulling upstream updates

```bash
git subtree pull --prefix=dependencies/BehaviorTreeCPP btCPP_upstream master --squash
```

### Pushing local changes upstream (if contributing back)

```bash
git subtree push --prefix=dependencies/BehaviorTreeCPP btCPP_upstream <branch>
```

### Making local modifications

Edit files directly under `dependencies/BehaviorTreeCPP/`. Commit them normally.
The subtree model keeps the full history in this repository.

## Environment

| Item       | Value              |
|------------|--------------------|
| OS         | Ubuntu 22.04       |
| ROS Distro | ROS2 Humble        |
| BT.CPP     | v4.9.0 (subtree)   |
| C++        | C++17              |
