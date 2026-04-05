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
  dev_scripts/                 <-- Development tools (bt_assistant, btstudio generator)
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
    launch/                    <-- ROS2 launch files
    test/                      <-- GTest unit and integration tests
    trees/                     <-- BT XML files run by tree_executor
      example_tree.xml         <-- Basic tree using ExampleAction/ExampleCondition
      action_server_demo.xml   <-- Demo tree for verifying the action server
```

## Build System

The repository contains two colcon/ament_cmake packages:

| Package             | Location                          | Description                    |
|---------------------|-----------------------------------|--------------------------------|
| behaviortree_cpp    | dependencies/BehaviorTreeCPP/     | BT.CPP v4 library (subtree)   |
| mercury_msgs        | mercury_common/mercury_msgs/      | Custom actions/messages        |
| mercury_autonomy    | mercury_autonomy/                 | Main autonomy package          |

colcon discovers both automatically. mercury_autonomy declares a `<depend>` on
`behaviortree_cpp` in its `package.xml` so colcon builds the library first.

### Building

From the workspace root (the directory containing `src/`):

```bash
colcon build --packages-select mercury_msgs behaviortree_cpp mercury_autonomy \
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
3. Provides an `autonomy/execute_tree` ROS2 action server
   (`mercury_msgs/action/ExecuteTree`) for goal-driven tree execution with
   feedback and cancellation. Execution runs on a background thread so the
   ROS2 executor remains responsive.
4. Publishes status updates on `autonomy/status`.
5. Tick rate is configurable via the `tick_rate_hz` parameter (default 30.0).
6. Calls `MercuryBtNode::staticDeinit()` after each tree execution to release
   shared resources (TF buffer/listener).
7. Rejects concurrent goals -- only one tree can execute at a time.

#### ExecuteTree Action Interface

| Field                  | Type    | Description                                  |
|------------------------|---------|----------------------------------------------|
| **Goal**               |         |                                              |
| tree_path              | string  | BT XML filename or absolute path             |
| **Result**             |         |                                              |
| return_code            | int32   | 0=SUCCESS, 1=FAILURE, 2=CANCELED, 3=ERROR    |
| **Feedback**           |         |                                              |
| current_status         | string  | Current BT status (e.g., "RUNNING")          |
| stack                  | TreeStack | Active tree call stack and node id         |
| elapsed_seconds        | float64 | Time since execution started                 |

#### Triggering a Tree

The `tree_path` goal field accepts either an absolute path or a filename.
Relative names are resolved by searching the configured tree directories
(installed share path, `tree_directory` parameter, `extra_tree_dirs`).

```bash
# List available trees
ros2 service call /mercury/autonomy/list_trees std_srvs/srv/Trigger

# Execute a tree by filename (resolved from tree directories)
ros2 action send_goal --feedback /mercury/autonomy/execute_tree \
    mercury_msgs/action/ExecuteTree "{tree_path: 'action_server_demo.xml'}"

# Execute a tree by absolute path
ros2 action send_goal --feedback /mercury/autonomy/execute_tree \
    mercury_msgs/action/ExecuteTree "{tree_path: '/full/path/to/tree.xml'}"

# Monitor execution status
ros2 topic echo /mercury/autonomy/status
```

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

### Using the BT Assistant

The `dev_scripts/bt_assistant.py` tool automates the above process:

```bash
# Run from the repository root
python3 dev_scripts/bt_assistant.py create action MoveToWaypoint
python3 dev_scripts/bt_assistant.py create condition IsSubmerged
python3 dev_scripts/bt_assistant.py create decorator RetryOnFail
```

Other commands:

```bash
python3 dev_scripts/bt_assistant.py list    # List all nodes by type
python3 dev_scripts/bt_assistant.py check   # Verify header/source/registration consistency
```

## Launch

The package provides a launch file at `launch/autonomy.launch.py`:

```bash
ros2 launch mercury_autonomy autonomy.launch.py
ros2 launch mercury_autonomy autonomy.launch.py robot:=mercury
```

Arguments:
- `robot` (default: `mercury`) -- namespace for all nodes.
- `tree_directory` (default: empty) -- override the tree XML search directory.
- `tick_rate_hz` (default: `30.0`) -- BT tick rate in Hz.

## Testing

Tests live in `test/` and use ament_cmake_gtest.

### Running tests

```bash
# Build with testing enabled
colcon build --packages-select mercury_autonomy \
    --cmake-args -DBUILD_TESTING=ON -DBTCPP_EXAMPLES=OFF -DBTCPP_BUILD_TOOLS=OFF

# Run
colcon test --packages-select mercury_autonomy
colcon test-result --verbose
```

### Test files

| Test                      | Description                                         |
|---------------------------|-----------------------------------------------------|
| test_tree_executor.cpp    | TreeExecutor ROS2 node integration tests            |
| test_approx_equal_to.cpp  | ApproxEqualTo condition factory & port verification |
| test_topic_nodes.cpp      | GetBoolTopic / GetFloatTopic registration & ports   |
| test_service_nodes.cpp    | CallSetBoolService / CallTriggerService reg. & ports|
| test_transform_pose.cpp   | TransformPose registration & port verification      |
| test_sensor_nodes.cpp     | GetOdometry / GetImuOrientation / GetTwistTopic     |
| test_condition_nodes.cpp  | ApproxEqualToAngle / CompareNums / IsTrue           |
| test_decorator_nodes.cpp  | RetryUntilSuccessfulOrTimeout reg. & ports          |
| test_publish_nodes.cpp    | PublishTwist / PublishBool registration & ports      |

### Adding a new test

1. Create `test/test_<name>.cpp`.
2. Add `ament_add_gtest(...)` in the `BUILD_TESTING` section of CMakeLists.txt.
3. Link against `mercury_autonomy_core` and any needed source files.

## Custom Nodes

### Actions

| Node                | Description                                              |
|---------------------|----------------------------------------------------------|
| ExampleAction       | Template action node (multi-tick lifecycle demo)         |
| GetBoolTopic        | Subscribe to a Bool topic and output the value           |
| GetFloatTopic       | Subscribe to a Float64 topic and output the value        |
| GetFloat64Topic     | Legacy alias that subscribes to a Float64 topic          |
| GetTwistTopic       | Subscribe to a Twist topic and output velocity components|
| GetOdometry         | Subscribe to an Odometry topic and output pose as XYZ/RPY|
| GetImuOrientation   | Subscribe to an IMU topic and output orientation as RPY  |
| GetMappingState     | Subscribe to mapping state and output the target name    |
| getCovariance       | Compute a scalar covariance score from pose covariance   |
| CallSetBoolService  | Asynchronously call a SetBool service with timeout       |
| CallTriggerService  | Asynchronously call a Trigger service with timeout       |
| SetMappingTarget    | Call the mapping target service with target and lock     |
| SetControllerSafeMode | Toggle controller stunt safe mode via parameters       |
| PublishToController | Publish controller setpoints to the linear/angular topics|
| PublishEKFPose      | Push a pose update to robot_localization/set_pose         |
| PublishInt8         | Publish an integer command topic for legacy trees        |
| SetStatus           | Publish a LED status command                              |
| TriggerControllerStunt | Trigger and confirm a controller stunt state          |
| Wait                | Wait for a fixed number of seconds                        |
| WaitForDetection    | Wait until a detection for a named object arrives        |
| ComputeFrameAlignment | Compute a controller pose by chaining TF transforms     |
| TransformPose       | Look up a TF2 transform and apply it to a pose          |
| PublishTwist        | Publish a Twist message with configurable velocity fields|
| PublishBool         | Publish a Bool message to a topic                        |

The `GetActuatorStatus` node from riptide is intentionally not ported yet
because Mercury does not currently have the required custom `ActuatorStatus.msg`
type. That message needs claw, torpedo, dropper, and busy state fields before
the node can be restored.

### Conditions

| Node                | Description                                              |
|---------------------|----------------------------------------------------------|
| ExampleCondition    | Template condition node                                  |
| ApproxEqualTo       | Returns SUCCESS if |a - b| < range                      |
| ApproxEqualToAngle  | Angle-aware approximate comparison (wraps to [-pi, pi]) |
| CompareNums         | Numeric comparison with configurable operator            |
| IsTrue              | Returns SUCCESS if the input boolean is true             |

### Decorators

| Node                            | Description                                   |
|---------------------------------|-----------------------------------------------|
| ExampleDecorator                | Template decorator node                       |
| RetryUntilSuccessfulOrTimeout   | Retry child until SUCCESS or a timeout elapses|

## Robot Namespace

The system supports configurable robot namespaces via standard ROS2 mechanisms.

### How it works

1. The launch file uses `PushRosNamespace` with a configurable `robot` argument
   (default: `mercury`). All nodes end up under `/<robot>/`.
2. Topic and service names in BT XML files should use **relative names** (no
   leading `/`). ROS2 automatically prefixes them with the node namespace.
3. `initRosForTree()` writes the robot namespace to the blackboard as
   `robot_ns`. BT XML trees can reference it via `{robot_ns}` for cases where
   the namespace string is needed directly (e.g., TF frame names).

### Example BT XML usage

```xml
<!-- Relative topic -- resolves to /mercury/odometry/filtered when robot=mercury -->
<GetOdometry topic="odometry/filtered" x="{x}" y="{y}" z="{z}" />

<!-- Using robot_ns for TF frame names -->
<TransformPose from_frame="{robot_ns}/base_link"
               to_frame="world"
               out_x="{tx}" out_y="{ty}" out_z="{tz}" />
```

### Changing the robot namespace

```bash
ros2 launch mercury_autonomy autonomy.launch.py robot:=talos
```

All topics, services, and the `robot_ns` blackboard entry will update
automatically.

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

## btstudio Integration

The package provides tooling to keep the visual BT editor
[btstudio](https://github.com/osu-uwrt/btstudio) in sync with the C++ node
definitions.

### Node Definition Generator

`dev_scripts/generate_btstudio_nodes.py` parses the C++ headers and emits both
a JSON manifest and a TypeScript file that can be imported into btstudio.

```bash
# Generate both JSON and TypeScript (default output: dev_scripts/)
python3 dev_scripts/generate_btstudio_nodes.py

# Generate only the TypeScript file
python3 dev_scripts/generate_btstudio_nodes.py --ts-only

# Custom output directory
python3 dev_scripts/generate_btstudio_nodes.py -o /path/to/btstudio/src/data/
```

### Applying to btstudio

1. Run the generator to produce `mercury_autonomy_nodes.ts`.
2. Copy the file into the btstudio `src/data/` directory.
3. In `nodeLibrary.ts`, import and spread:
   ```ts
   import { mercuryAutonomyNodes } from './mercury_autonomy_nodes';
   export const nodeLibrary: BTNodeDefinition[] = [
     ...existingNodes,
     ...mercuryAutonomyNodes,
   ];
   ```
4. Rebuild btstudio.

Re-run the generator whenever nodes are added or ports change.

## Environment

| Item       | Value              |
|------------|--------------------|
| OS         | Ubuntu 22.04       |
| ROS Distro | ROS2 Humble        |
| BT.CPP     | v4.9.0 (subtree)   |
| C++        | C++17              |
