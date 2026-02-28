#include "mercury_autonomy/tree_manager.h"
#include <rclcpp/rclcpp.hpp>
#include <chrono>

namespace mercury_autonomy
{

TreeManager::TreeManager(rclcpp::Node::SharedPtr node, double tick_rate)
: node_(node), tick_rate_(tick_rate)
{
}

TreeManager::~TreeManager()
{
  stop_execution();
}

bool TreeManager::load_tree(BT::Tree && tree)
{
  tree_ = std::move(tree);
  current_status_ = BT::NodeStatus::IDLE;
  RCLCPP_INFO(node_->get_logger(), "Tree loaded successfully");
  return true;
}

bool TreeManager::start_execution()
{
  if (!tree_) {
    RCLCPP_ERROR(node_->get_logger(), "No tree loaded, cannot start execution");
    return false;
  }

  if (running_) {
    RCLCPP_WARN(node_->get_logger(), "Tree execution already running");
    return false;
  }

  running_ = true;
  pause_requested_ = false;

  try {
    execution_thread_ = std::thread(&TreeManager::execution_loop, this);
    RCLCPP_INFO(node_->get_logger(), "Tree execution started");
    return true;
  } catch (const std::exception & e) {
    RCLCPP_ERROR(node_->get_logger(), "Failed to start execution: %s", e.what());
    running_ = false;
    return false;
  }
}

bool TreeManager::stop_execution()
{
  if (!running_) {
    return true;
  }

  running_ = false;
  pause_requested_ = false;
  pause_cv_.notify_all();

  if (execution_thread_.joinable()) {
    execution_thread_.join();
  }

  RCLCPP_INFO(node_->get_logger(), "Tree execution stopped");
  return true;
}

BT::NodeStatus TreeManager::get_tree_status() const
{
  return current_status_.load();
}

bool TreeManager::is_running() const
{
  return running_.load();
}

void TreeManager::request_pause()
{
  pause_requested_ = true;
  RCLCPP_INFO(node_->get_logger(), "Pause requested");
}

void TreeManager::resume_execution()
{
  if (pause_requested_) {
    pause_requested_ = false;
    pause_cv_.notify_all();
    RCLCPP_INFO(node_->get_logger(), "Execution resumed");
  }
}

void TreeManager::execution_loop()
{
  std::chrono::duration<double> period(1.0 / tick_rate_);

  while (running_) {
    // Handle pause requests
    {
      std::unique_lock<std::mutex> lock(pause_mutex_);
      while (pause_requested_ && running_) {
        pause_cv_.wait(lock);
      }
    }

    if (!running_) {
      break;
    }

    // Tick the tree
    try {
      if (tree_) {
        current_status_ = tree_->tickOnce();

        // Log status changes
        if (current_status_ == BT::NodeStatus::SUCCESS) {
          RCLCPP_DEBUG(node_->get_logger(), "Tree execution succeeded");
        } else if (current_status_ == BT::NodeStatus::FAILURE) {
          RCLCPP_DEBUG(node_->get_logger(), "Tree execution failed");
        }
      }
    } catch (const std::exception & e) {
      RCLCPP_ERROR(node_->get_logger(), "Exception during tree tick: %s", e.what());
      current_status_ = BT::NodeStatus::FAILURE;
      break;
    }

    // Sleep for the remaining time
    std::this_thread::sleep_for(period);
  }
}

}  // namespace mercury_autonomy
