cmake_minimum_required(VERSION 3.8)
project(mercury_autonomy)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(vision_msgs REQUIRED)
find_package(robot_localization REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(behaviortree_cpp REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(mercury_msgs REQUIRED)

set(THIS_PACKAGE_DEPS
  rclcpp
  rclcpp_action
  std_msgs
  std_srvs
  geometry_msgs
  sensor_msgs
  nav_msgs
  vision_msgs
  robot_localization
  tf2
  tf2_ros
  tf2_geometry_msgs
  behaviortree_cpp
  ament_index_cpp
  mercury_msgs
)

# ---------------------------------------------------------------------------
# Core utility library -- shared helpers and the ROS-BT bridge
# ---------------------------------------------------------------------------
add_library(mercury_autonomy_core SHARED
  src/autonomy_util.cpp
  src/mercury_bt_node.cpp
)

target_include_directories(mercury_autonomy_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(mercury_autonomy_core ${THIS_PACKAGE_DEPS})

# ---------------------------------------------------------------------------
# Action node plugin library
# ---------------------------------------------------------------------------
file(GLOB action_sources CONFIGURE_DEPENDS
  src/bt_actions/*.cpp
)

add_library(mercury_bt_actions SHARED
  ${action_sources}
  src/register_actions.cpp
)

target_include_directories(mercury_bt_actions PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(mercury_bt_actions ${THIS_PACKAGE_DEPS})
target_link_libraries(mercury_bt_actions mercury_autonomy_core)
target_compile_definitions(mercury_bt_actions PRIVATE BT_PLUGIN_EXPORT)

# ---------------------------------------------------------------------------
# Condition node plugin library
# ---------------------------------------------------------------------------
file(GLOB condition_sources CONFIGURE_DEPENDS
  src/bt_conditions/*.cpp
)

add_library(mercury_bt_conditions SHARED
  ${condition_sources}
  src/register_conditions.cpp
)

target_include_directories(mercury_bt_conditions PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(mercury_bt_conditions ${THIS_PACKAGE_DEPS})
target_link_libraries(mercury_bt_conditions mercury_autonomy_core)
target_compile_definitions(mercury_bt_conditions PRIVATE BT_PLUGIN_EXPORT)

# ---------------------------------------------------------------------------
# Decorator node plugin library
# ---------------------------------------------------------------------------
file(GLOB decorator_sources CONFIGURE_DEPENDS
  src/bt_decorators/*.cpp
)

add_library(mercury_bt_decorators SHARED
  ${decorator_sources}
  src/register_decorators.cpp
)

target_include_directories(mercury_bt_decorators PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(mercury_bt_decorators ${THIS_PACKAGE_DEPS})
target_link_libraries(mercury_bt_decorators mercury_autonomy_core)
target_compile_definitions(mercury_bt_decorators PRIVATE BT_PLUGIN_EXPORT)

# ---------------------------------------------------------------------------
# Tree executor -- the ROS2 action server that runs BT XML files
# ---------------------------------------------------------------------------
add_executable(tree_executor src/tree_executor.cpp)

target_include_directories(tree_executor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_compile_definitions(tree_executor PUBLIC
  AUTONOMY_PKG_NAME="${PROJECT_NAME}"
)

ament_target_dependencies(tree_executor ${THIS_PACKAGE_DEPS})
target_link_libraries(tree_executor mercury_autonomy_core)

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------
install(TARGETS tree_executor
  DESTINATION lib/${PROJECT_NAME}
)

install(TARGETS
  mercury_autonomy_core
  mercury_bt_actions
  mercury_bt_conditions
  mercury_bt_decorators
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY trees/
  DESTINATION share/${PROJECT_NAME}/trees
)

install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
)

# ---------------------------------------------------------------------------
# Testing
# ---------------------------------------------------------------------------
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()

  find_package(ament_cmake_gtest REQUIRED)

  # Tree executor integration test
  ament_add_gtest(test_tree_executor test/test_tree_executor.cpp)
  target_include_directories(test_tree_executor PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  ament_target_dependencies(test_tree_executor ${THIS_PACKAGE_DEPS})
  target_link_libraries(test_tree_executor mercury_autonomy_core)

  # ApproxEqualTo unit test
  ament_add_gtest(test_approx_equal_to test/test_approx_equal_to.cpp
    src/bt_conditions/approx_equal_to.cpp
  )
  target_include_directories(test_approx_equal_to PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  ament_target_dependencies(test_approx_equal_to ${THIS_PACKAGE_DEPS})
  target_link_libraries(test_approx_equal_to mercury_autonomy_core)

  # Topic node (GetBoolTopic / GetFloatTopic) unit test
  ament_add_gtest(test_topic_nodes test/test_topic_nodes.cpp
    src/bt_actions/get_bool_topic.cpp
    src/bt_actions/get_float_topic.cpp
  )
  target_include_directories(test_topic_nodes PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  ament_target_dependencies(test_topic_nodes ${THIS_PACKAGE_DEPS})
  target_link_libraries(test_topic_nodes mercury_autonomy_core)

  # Service node (CallSetBoolService / CallTriggerService) unit test
  ament_add_gtest(test_service_nodes test/test_service_nodes.cpp
    src/bt_actions/call_set_bool_service.cpp
    src/bt_actions/call_trigger_service.cpp
  )
  target_include_directories(test_service_nodes PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  ament_target_dependencies(test_service_nodes ${THIS_PACKAGE_DEPS})
  target_link_libraries(test_service_nodes mercury_autonomy_core)

  # TransformPose unit test
  ament_add_gtest(test_transform_pose test/test_transform_pose.cpp
    src/bt_actions/transform_pose.cpp
  )
  target_include_directories(test_transform_pose PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  ament_target_dependencies(test_transform_pose ${THIS_PACKAGE_DEPS})
  target_link_libraries(test_transform_pose mercury_autonomy_core)

  # Sensor subscriber node (GetOdometry / GetImuOrientation / GetTwistTopic) test
  ament_add_gtest(test_sensor_nodes test/test_sensor_nodes.cpp
    src/bt_actions/get_odometry.cpp
    src/bt_actions/get_imu_orientation.cpp
    src/bt_actions/get_twist_topic.cpp
  )
  target_include_directories(test_sensor_nodes PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  ament_target_dependencies(test_sensor_nodes ${THIS_PACKAGE_DEPS})
  target_link_libraries(test_sensor_nodes mercury_autonomy_core)

  # Condition node (ApproxEqualToAngle / CompareNums / IsTrue) unit test
  ament_add_gtest(test_condition_nodes test/test_condition_nodes.cpp
    src/bt_conditions/approx_equal_to_angle.cpp
    src/bt_conditions/compare_nums.cpp
    src/bt_conditions/is_true.cpp
  )
  target_include_directories(test_condition_nodes PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  ament_target_dependencies(test_condition_nodes ${THIS_PACKAGE_DEPS})
  target_link_libraries(test_condition_nodes mercury_autonomy_core)

  # Decorator node (RetryUntilSuccessfulOrTimeout) unit test
  ament_add_gtest(test_decorator_nodes test/test_decorator_nodes.cpp
    src/bt_decorators/retry_until_successful_or_timeout.cpp
  )
  target_include_directories(test_decorator_nodes PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  ament_target_dependencies(test_decorator_nodes ${THIS_PACKAGE_DEPS})
  target_link_libraries(test_decorator_nodes mercury_autonomy_core)

  # Publish node (PublishTwist / PublishBool) unit test
  ament_add_gtest(test_publish_nodes test/test_publish_nodes.cpp
    src/bt_actions/publish_twist.cpp
    src/bt_actions/publish_bool.cpp
  )
  target_include_directories(test_publish_nodes PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  ament_target_dependencies(test_publish_nodes ${THIS_PACKAGE_DEPS})
  target_link_libraries(test_publish_nodes mercury_autonomy_core)

  # Ported action node registration and port tests
  ament_add_gtest(test_ported_action_nodes test/test_ported_action_nodes.cpp)
  target_include_directories(test_ported_action_nodes PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  ament_target_dependencies(test_ported_action_nodes ${THIS_PACKAGE_DEPS})
  target_link_libraries(test_ported_action_nodes mercury_autonomy_core mercury_bt_actions)
endif()

ament_package()
