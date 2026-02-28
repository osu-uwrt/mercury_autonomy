#ifndef MERCURY_AUTONOMY__TREE_MANAGER_H_
#define MERCURY_AUTONOMY__TREE_MANAGER_H_

#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>

#include <rclcpp/rclcpp.hpp>
#include <behaviortree_cpp/bt_factory.h>

namespace mercury_autonomy
{

/**
 * @class TreeManager
 * @brief Manages the execution of behavior trees in a separate thread
 *
 * This class handles the lifecycle and execution of BehaviorTree.CPP trees,
 * providing thread-safe control over tree execution with rate limiting.
 */
class TreeManager
{
public:
  /**
   * @brief Constructor for TreeManager
   * @param node Shared pointer to the ROS2 node
   * @param tick_rate Rate at which to tick the tree (Hz)
   */
  TreeManager(rclcpp::Node::SharedPtr node, double tick_rate = 10.0);

  /**
   * @brief Destructor for TreeManager
   */
  ~TreeManager();

  /**
   * @brief Load and initialize a behavior tree
   * @param tree Behavior tree to manage
   * @return True if loading was successful
   */
  bool load_tree(BT::Tree && tree);

  /**
   * @brief Start the tree execution in a background thread
   * @return True if successfully started
   */
  bool start_execution();

  /**
   * @brief Stop the tree execution
   * @return True if successfully stopped
   */
  bool stop_execution();

  /**
   * @brief Get the current status of the tree
   * @return The BehaviorTree status
   */
  BT::NodeStatus get_tree_status() const;

  /**
   * @brief Check if the tree is currently running
   * @return True if tree execution thread is active
   */
  bool is_running() const;

  /**
   * @brief Request a pause of tree execution
   */
  void request_pause();

  /**
   * @brief Resume tree execution from pause
   */
  void resume_execution();

  /**
   * @brief Get the tick rate of the tree
   * @return Tick rate in Hz
   */
  double get_tick_rate() const { return tick_rate_; }

  /**
   * @brief Set the tick rate of the tree
   * @param tick_rate New tick rate in Hz
   */
  void set_tick_rate(double tick_rate) { tick_rate_ = tick_rate; }

protected:
  rclcpp::Node::SharedPtr node_;
  std::optional<BT::Tree> tree_;
  std::thread execution_thread_;
  std::atomic<bool> running_{false};
  std::atomic<bool> pause_requested_{false};
  std::atomic<BT::NodeStatus> current_status_{BT::NodeStatus::IDLE};
  double tick_rate_;

  std::mutex pause_mutex_;
  std::condition_variable pause_cv_;

  /**
   * @brief Main execution loop (runs in background thread)
   */
  void execution_loop();
};

}  // namespace mercury_autonomy

#endif  // MERCURY_AUTONOMY__TREE_MANAGER_H_
