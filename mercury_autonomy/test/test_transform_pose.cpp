// Unit tests for TransformPose action node.
// Verifies factory registration and port definitions.

#include <gtest/gtest.h>

#include <behaviortree_cpp/behavior_tree.h>
#include <behaviortree_cpp/bt_factory.h>

#include "mercury_autonomy/bt_actions/transform_pose.hpp"

class TransformPoseTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    factory_.registerNodeType<mercury_autonomy::TransformPose>("TransformPose");
  }

  BT::BehaviorTreeFactory factory_;
};

TEST_F(TransformPoseTest, RegistersWithFactory)
{
  auto manifests = factory_.manifests();
  ASSERT_TRUE(manifests.count("TransformPose") > 0);
}

TEST_F(TransformPoseTest, HasCorrectPorts)
{
  auto manifests = factory_.manifests();
  auto it = manifests.find("TransformPose");
  ASSERT_NE(it, manifests.end());

  const auto & ports = it->second.ports;
  EXPECT_TRUE(ports.count("from_frame") > 0);
  EXPECT_TRUE(ports.count("to_frame") > 0);
  EXPECT_TRUE(ports.count("x") > 0);
  EXPECT_TRUE(ports.count("y") > 0);
  EXPECT_TRUE(ports.count("z") > 0);
  EXPECT_TRUE(ports.count("roll") > 0);
  EXPECT_TRUE(ports.count("pitch") > 0);
  EXPECT_TRUE(ports.count("yaw") > 0);
  EXPECT_TRUE(ports.count("timeout") > 0);
  EXPECT_TRUE(ports.count("out_x") > 0);
  EXPECT_TRUE(ports.count("out_y") > 0);
  EXPECT_TRUE(ports.count("out_z") > 0);
  EXPECT_TRUE(ports.count("out_roll") > 0);
  EXPECT_TRUE(ports.count("out_pitch") > 0);
  EXPECT_TRUE(ports.count("out_yaw") > 0);
}
