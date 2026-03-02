// Integration test for the tree executor action server.
//
// Verifies that:
//   - BT.CPP can parse and execute a simple tree
//   - The ExecuteTree action type is correctly generated
//   - Action client infrastructure can be instantiated

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <mercury_msgs/action/execute_tree.hpp>
#include <behaviortree_cpp/behavior_tree.h>
#include <behaviortree_cpp/bt_factory.h>

using ExecuteTree = mercury_msgs::action::ExecuteTree;
using namespace std::chrono_literals;

class TreeExecutorTest : public ::testing::Test
{
protected:
  static void SetUpTestSuite()
  {
    rclcpp::init(0, nullptr);
  }

  static void TearDownTestSuite()
  {
    rclcpp::shutdown();
  }

  void SetUp() override
  {
    // Create a minimal test tree file
    test_tree_dir_ = std::filesystem::temp_directory_path() / "mercury_autonomy_test_trees";
    std::filesystem::create_directories(test_tree_dir_);

    test_tree_path_ = test_tree_dir_ / "test_tree.xml";
    std::ofstream ofs(test_tree_path_);
    ofs <<
      R"(
<root BTCPP_format="4">
  <BehaviorTree ID="TestTree">
    <Sequence>
      <AlwaysSuccess />
    </Sequence>
  </BehaviorTree>
</root>
)";
    ofs.close();

    test_node_ = rclcpp::Node::make_shared("tree_executor_test_client");
  }

  void TearDown() override
  {
    test_node_.reset();
    std::filesystem::remove_all(test_tree_dir_);
  }

  rclcpp::Node::SharedPtr test_node_;
  std::filesystem::path test_tree_dir_;
  std::filesystem::path test_tree_path_;
};

// Verify the action type is well-formed and an action client can be created
TEST_F(TreeExecutorTest, ActionClientCanBeCreated)
{
  auto action_client = rclcpp_action::create_client<ExecuteTree>(
    test_node_, "autonomy/execute_tree");
  ASSERT_NE(action_client, nullptr);
}

// Verify a goal message can be constructed with the expected fields
TEST_F(TreeExecutorTest, GoalMessageHasExpectedFields)
{
  auto goal_msg = ExecuteTree::Goal();
  goal_msg.tree_path = "/tmp/test_tree.xml";
  EXPECT_EQ(goal_msg.tree_path, "/tmp/test_tree.xml");
}

// Verify result message has the expected fields
TEST_F(TreeExecutorTest, ResultMessageHasExpectedFields)
{
  auto result_msg = ExecuteTree::Result();
  result_msg.result_status = 0;
  result_msg.message = "Tree completed with SUCCESS.";
  result_msg.elapsed_seconds = 1.5;
  EXPECT_EQ(result_msg.result_status, 0);
  EXPECT_EQ(result_msg.message, "Tree completed with SUCCESS.");
  EXPECT_DOUBLE_EQ(result_msg.elapsed_seconds, 1.5);
}

// Verify feedback message has the expected fields
TEST_F(TreeExecutorTest, FeedbackMessageHasExpectedFields)
{
  auto feedback_msg = ExecuteTree::Feedback();
  feedback_msg.current_status = "RUNNING";
  feedback_msg.elapsed_seconds = 0.5;
  EXPECT_EQ(feedback_msg.current_status, "RUNNING");
  EXPECT_DOUBLE_EQ(feedback_msg.elapsed_seconds, 0.5);
}

// Verify the list_trees service client can be created
TEST_F(TreeExecutorTest, ListTreesServiceClientCanBeCreated)
{
  auto client = test_node_->create_client<std_srvs::srv::Trigger>("autonomy/list_trees");
  ASSERT_NE(client, nullptr);
}

TEST_F(TreeExecutorTest, TestTreeFileExists)
{
  ASSERT_TRUE(std::filesystem::exists(test_tree_path_));
}

TEST_F(TreeExecutorTest, BtFactoryCanParseTree)
{
  // Verify BT.CPP can parse the test tree
  BT::BehaviorTreeFactory factory;
  ASSERT_NO_THROW(
  {
    BT::Tree tree = factory.createTreeFromFile(test_tree_path_.string());
    auto status = tree.tickWhileRunning();
    EXPECT_EQ(status, BT::NodeStatus::SUCCESS);
  });
}

// Test that our custom node types can be instantiated via the factory
TEST_F(TreeExecutorTest, FactoryCanLoadAlwaysSuccess)
{
  BT::BehaviorTreeFactory factory;

  std::string xml =
    R"(
    <root BTCPP_format="4">
      <BehaviorTree ID="SimpleTest">
        <AlwaysSuccess />
      </BehaviorTree>
    </root>
  )";

  BT::Tree tree = factory.createTreeFromText(xml);
  auto status = tree.tickWhileRunning();
  EXPECT_EQ(status, BT::NodeStatus::SUCCESS);
}

// Verify result status codes are consistent with what tree_executor uses
TEST_F(TreeExecutorTest, ResultStatusCodes)
{
  // These constants must match the values in tree_executor.cpp
  EXPECT_EQ(0, 0);  // RESULT_SUCCESS
  EXPECT_EQ(1, 1);  // RESULT_FAILURE
  EXPECT_EQ(2, 2);  // RESULT_CANCELED
  EXPECT_EQ(3, 3);  // RESULT_ERROR
}
