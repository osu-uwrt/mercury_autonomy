#include "mercury_autonomy/autonomy_core.h"
#include <rclcpp/rclcpp.hpp>

namespace mercury_autonomy
{

AutonomyCore::AutonomyCore(rclcpp::Node::SharedPtr node)
: node_(node)
{
}

bool AutonomyCore::initialize()
{
  try {
    RCLCPP_INFO(node_->get_logger(), "AutonomyCore initialized successfully");
    return true;
  } catch (const std::exception & e) {
    RCLCPP_ERROR(node_->get_logger(), "Failed to initialize AutonomyCore: %s", e.what());
    return false;
  }
}

BT::Tree AutonomyCore::load_tree(const std::string & xml_file)
{
  try {
    auto tree = factory_.createTreeFromFile(xml_file);
    RCLCPP_INFO(node_->get_logger(), "Successfully loaded tree from: %s", xml_file.c_str());
    return tree;
  } catch (const std::exception & e) {
    RCLCPP_ERROR(
      node_->get_logger(), "Failed to load tree from %s: %s", xml_file.c_str(),
      e.what());
    throw;
  }
}

}  // namespace mercury_autonomy
