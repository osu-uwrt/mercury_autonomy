"""
Launch file for the mercury_autonomy tree executor.

Usage:
  ros2 launch mercury_autonomy autonomy.launch.py
  ros2 launch mercury_autonomy autonomy.launch.py robot:=mercury
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

    # Tree executor node
    tree_executor_node = Node(
        package="mercury_autonomy",
        executable="tree_executor",
        name="tree_executor",
        output="screen",
        parameters=[
            {"tree_directory": LaunchConfiguration("tree_directory")},
            {"extra_plugins": []},
            {"extra_tree_dirs": []},
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
        autonomy_group,
    ])
