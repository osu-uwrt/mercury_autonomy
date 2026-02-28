cmake_minimum_required(VERSION 3.16)
project(mercury_autonomy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find ROS2 and required packages
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(behaviortree_cpp REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Main library - Core functionality for autonomy
add_library(mercury_autonomy_lib SHARED
  src/autonomy_core.cpp
  src/tree_manager.cpp
  src/action_server.cpp
)

ament_target_dependencies(mercury_autonomy_lib
  rclcpp
  rclcpp_action
  rclcpp_components
  behaviortree_cpp
)

target_include_directories(mercury_autonomy_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Executable - Action server node that runs a behavior tree
add_executable(autonomy_action_server
  src/autonomy_action_server_main.cpp
)

target_link_libraries(autonomy_action_server
  mercury_autonomy_lib
)

ament_target_dependencies(autonomy_action_server
  rclcpp
  rclcpp_action
  behaviortree_cpp
)

# Register as a ROS component
rclcpp_components_register_node(mercury_autonomy_lib
  PLUGIN "mercury_autonomy::AutonomyComponent"
  EXECUTABLE autonomy_component_node
)

# Install targets
install(TARGETS
  mercury_autonomy_lib
  autonomy_action_server
  autonomy_component_node
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

# Install headers
install(DIRECTORY include/ DESTINATION include)

# Install trees directory (for BT XML files)
install(DIRECTORY trees/ DESTINATION share/${PROJECT_NAME}/trees OPTIONAL)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
