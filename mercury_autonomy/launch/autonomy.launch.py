"""
Launch file for the mercury_autonomy tree executor.

Usage:
  ros2 launch mercury_autonomy autonomy.launch.py
  ros2 launch mercury_autonomy autonomy.launch.py robot:=mercury
  ros2 launch mercury_autonomy autonomy.launch.py tick_rate_hz:=50.0
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, GroupAction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node, PushRosNamespace


def generate_launch_description():
    # Declare arguments
    robot_arg = DeclareLaunchArgument(
        "robot",
        default_value="mercury",
        description="Robot namespace to push all nodes under",
    )

    tree_dir_arg = DeclareLaunchArgument(
        "tree_directory",
        default_value="",
        description="Optional override for the tree XML search directory",
    )

    tick_rate_arg = DeclareLaunchArgument(
        "tick_rate_hz",
        default_value="30.0",
        description="BT tick rate in Hz (default 30.0)",
    )

    # Tree executor node
    # Note: extra_plugins and extra_tree_dirs are string-array parameters
    # with empty defaults declared in the C++ node. Only override them in the
    # launch file when a non-empty value is needed; passing an empty list here
    # causes a launch-time type error in ROS2 Humble.
    tree_executor_node = Node(
        package="mercury_autonomy",
        executable="tree_executor",
        name="tree_executor",
        output="screen",
        parameters=[
            {"tree_directory": LaunchConfiguration("tree_directory")},
            {"tick_rate_hz": LaunchConfiguration("tick_rate_hz")},
        ],
    )

    # Group everything under the robot namespace
    autonomy_group = GroupAction([
        PushRosNamespace(LaunchConfiguration("robot")),
        tree_executor_node,
    ])

    return LaunchDescription([
        robot_arg,
        tree_dir_arg,
        tick_rate_arg,
        autonomy_group,
    ])
