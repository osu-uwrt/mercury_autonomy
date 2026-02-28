// Unit tests for ApproxEqualToAngle, CompareNums, and IsTrue condition nodes.
// Verifies factory registration and port definitions.

#include <gtest/gtest.h>

#include <behaviortree_cpp/behavior_tree.h>
#include <behaviortree_cpp/bt_factory.h>

#include "mercury_autonomy/bt_conditions/approx_equal_to_angle.hpp"
#include "mercury_autonomy/bt_conditions/compare_nums.hpp"
#include "mercury_autonomy/bt_conditions/is_true.hpp"

class ConditionNodeTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    factory_.registerNodeType<mercury_autonomy::ApproxEqualToAngle>("ApproxEqualToAngle");
    factory_.registerNodeType<mercury_autonomy::CompareNums>("CompareNums");
    factory_.registerNodeType<mercury_autonomy::IsTrue>("IsTrue");
  }

  BT::BehaviorTreeFactory factory_;
};

// -- ApproxEqualToAngle --

TEST_F(ConditionNodeTest, ApproxEqualToAngleRegisters)
{
  auto manifests = factory_.manifests();
  ASSERT_TRUE(manifests.count("ApproxEqualToAngle") > 0);
}

TEST_F(ConditionNodeTest, ApproxEqualToAngleHasCorrectPorts)
{
  auto manifests = factory_.manifests();
  auto it = manifests.find("ApproxEqualToAngle");
  ASSERT_NE(it, manifests.end());

  const auto & ports = it->second.ports;
  EXPECT_TRUE(ports.count("a") > 0);
  EXPECT_TRUE(ports.count("b") > 0);
  EXPECT_TRUE(ports.count("range") > 0);
}

// -- CompareNums --

TEST_F(ConditionNodeTest, CompareNumsRegisters)
{
  auto manifests = factory_.manifests();
  ASSERT_TRUE(manifests.count("CompareNums") > 0);
}

TEST_F(ConditionNodeTest, CompareNumsHasCorrectPorts)
{
  auto manifests = factory_.manifests();
  auto it = manifests.find("CompareNums");
  ASSERT_NE(it, manifests.end());

  const auto & ports = it->second.ports;
  EXPECT_TRUE(ports.count("test") > 0);
  EXPECT_TRUE(ports.count("a") > 0);
  EXPECT_TRUE(ports.count("b") > 0);
}

// -- IsTrue --

TEST_F(ConditionNodeTest, IsTrueRegisters)
{
  auto manifests = factory_.manifests();
  ASSERT_TRUE(manifests.count("IsTrue") > 0);
}

TEST_F(ConditionNodeTest, IsTrueHasCorrectPorts)
{
  auto manifests = factory_.manifests();
  auto it = manifests.find("IsTrue");
  ASSERT_NE(it, manifests.end());

  const auto & ports = it->second.ports;
  EXPECT_TRUE(ports.count("value") > 0);
}
