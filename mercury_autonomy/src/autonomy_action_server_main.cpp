#include <rclcpp/rclcpp.hpp>
#include "mercury_autonomy/action_server.h"

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<mercury_autonomy::AutonomyActionServer>(
    "autonomy_action_server", "run_tree");

  if (!node->initialize()) {
    RCLCPP_ERROR(node->get_logger(), "Failed to initialize autonomy action server");
    return 1;
  }

  RCLCPP_INFO(node->get_logger(), "Autonomy Action Server started");
  rclcpp::spin(node);
  rclcpp::shutdown();

  return 0;
}
