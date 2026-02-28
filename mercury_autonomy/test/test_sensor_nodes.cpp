// Unit tests for sensor/topic subscriber action nodes.
// Verifies factory registration and port definitions for
// GetOdometry, GetImuOrientation, and GetTwistTopic.

#include <gtest/gtest.h>

#include <behaviortree_cpp/behavior_tree.h>
#include <behaviortree_cpp/bt_factory.h>

#include "mercury_autonomy/bt_actions/get_odometry.hpp"
#include "mercury_autonomy/bt_actions/get_imu_orientation.hpp"
#include "mercury_autonomy/bt_actions/get_twist_topic.hpp"

class SensorNodeTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    factory_.registerNodeType<mercury_autonomy::GetOdometry>("GetOdometry");
    factory_.registerNodeType<mercury_autonomy::GetImuOrientation>("GetImuOrientation");
    factory_.registerNodeType<mercury_autonomy::GetTwistTopic>("GetTwistTopic");
  }

  BT::BehaviorTreeFactory factory_;
};

// -- GetOdometry --

TEST_F(SensorNodeTest, GetOdometryRegisters)
{
  auto manifests = factory_.manifests();
  ASSERT_TRUE(manifests.count("GetOdometry") > 0);
}

TEST_F(SensorNodeTest, GetOdometryHasCorrectPorts)
{
  auto manifests = factory_.manifests();
  auto it = manifests.find("GetOdometry");
  ASSERT_NE(it, manifests.end());

  const auto & ports = it->second.ports;
  EXPECT_TRUE(ports.count("topic") > 0);
  EXPECT_TRUE(ports.count("timeout") > 0);
  EXPECT_TRUE(ports.count("x") > 0);
  EXPECT_TRUE(ports.count("y") > 0);
  EXPECT_TRUE(ports.count("z") > 0);
  EXPECT_TRUE(ports.count("roll") > 0);
  EXPECT_TRUE(ports.count("pitch") > 0);
  EXPECT_TRUE(ports.count("yaw") > 0);
}

// -- GetImuOrientation --

TEST_F(SensorNodeTest, GetImuOrientationRegisters)
{
  auto manifests = factory_.manifests();
  ASSERT_TRUE(manifests.count("GetImuOrientation") > 0);
}

TEST_F(SensorNodeTest, GetImuOrientationHasCorrectPorts)
{
  auto manifests = factory_.manifests();
  auto it = manifests.find("GetImuOrientation");
  ASSERT_NE(it, manifests.end());

  const auto & ports = it->second.ports;
  EXPECT_TRUE(ports.count("topic") > 0);
  EXPECT_TRUE(ports.count("timeout") > 0);
  EXPECT_TRUE(ports.count("roll") > 0);
  EXPECT_TRUE(ports.count("pitch") > 0);
  EXPECT_TRUE(ports.count("yaw") > 0);
}

// -- GetTwistTopic --

TEST_F(SensorNodeTest, GetTwistTopicRegisters)
{
  auto manifests = factory_.manifests();
  ASSERT_TRUE(manifests.count("GetTwistTopic") > 0);
}

TEST_F(SensorNodeTest, GetTwistTopicHasCorrectPorts)
{
  auto manifests = factory_.manifests();
  auto it = manifests.find("GetTwistTopic");
  ASSERT_NE(it, manifests.end());

  const auto & ports = it->second.ports;
  EXPECT_TRUE(ports.count("topic") > 0);
  EXPECT_TRUE(ports.count("timeout") > 0);
  EXPECT_TRUE(ports.count("vel_x") > 0);
  EXPECT_TRUE(ports.count("vel_y") > 0);
  EXPECT_TRUE(ports.count("vel_z") > 0);
  EXPECT_TRUE(ports.count("vel_roll") > 0);
  EXPECT_TRUE(ports.count("vel_pitch") > 0);
  EXPECT_TRUE(ports.count("vel_yaw") > 0);
}
