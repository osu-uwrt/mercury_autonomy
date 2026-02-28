// Core includes shared across the mercury_autonomy package.
// This header aggregates common ROS2 and BT.CPP headers to keep
// individual node headers concise.

#pragma once

// Standard library
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>

// BehaviorTree.CPP v4
#include <behaviortree_cpp/behavior_tree.h>
#include <behaviortree_cpp/bt_factory.h>
#include <behaviortree_cpp/loggers/groot2_publisher.h>

// ROS2 core
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>

// Ament index (for locating installed package resources)
#include <ament_index_cpp/get_package_prefix.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>

// TF2
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

// Common message types
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/string.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <geometry_msgs/msg/vector3.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/imu.hpp>

// Services
#include <std_srvs/srv/set_bool.hpp>
#include <std_srvs/srv/trigger.hpp>
