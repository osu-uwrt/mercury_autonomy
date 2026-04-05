// Registration and port checks for legacy riptide action node names.

#include <gtest/gtest.h>

#include <behaviortree_cpp/behavior_tree.h>
#include <behaviortree_cpp/bt_factory.h>

#include "mercury_autonomy/bt_actions/compute_frame_alignment.hpp"
#include "mercury_autonomy/bt_actions/get_covariance.hpp"
#include "mercury_autonomy/bt_actions/get_float64_topic.hpp"
#include "mercury_autonomy/bt_actions/get_mapping_state.hpp"
#include "mercury_autonomy/bt_actions/publish_ekf_pose.hpp"
#include "mercury_autonomy/bt_actions/publish_int8.hpp"
#include "mercury_autonomy/bt_actions/publish_to_controller.hpp"
#include "mercury_autonomy/bt_actions/set_controller_safe_mode.hpp"
#include "mercury_autonomy/bt_actions/set_mapping_target.hpp"
#include "mercury_autonomy/bt_actions/set_status.hpp"
#include "mercury_autonomy/bt_actions/trigger_controller_stunt.hpp"
#include "mercury_autonomy/bt_actions/wait.hpp"
#include "mercury_autonomy/bt_actions/wait_for_detection.hpp"

class LegacyActionNodeTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    factory_.registerNodeType<mercury_autonomy::GetFloat64Topic>("GetFloat64Topic");
    factory_.registerNodeType<mercury_autonomy::PublishInt8>("PublishInt8");
    factory_.registerNodeType<mercury_autonomy::GetMappingState>("GetMappingState");
    factory_.registerNodeType<mercury_autonomy::SetMappingTarget>("SetMappingTarget");
    factory_.registerNodeType<mercury_autonomy::PublishEKFPose>("PublishEKFPose");
    factory_.registerNodeType<mercury_autonomy::PublishToController>("PublishToController");
    factory_.registerNodeType<mercury_autonomy::SetControllerSafeMode>("SetControllerSafeMode");
    factory_.registerNodeType<mercury_autonomy::SetStatus>("SetStatus");
    factory_.registerNodeType<mercury_autonomy::TriggerControllerStunt>("TriggerControllerStunt");
    factory_.registerNodeType<mercury_autonomy::Wait>("Wait");
    factory_.registerNodeType<mercury_autonomy::WaitForDetection>("WaitForDetection");
    factory_.registerNodeType<mercury_autonomy::ComputeFrameAlignment>("ComputeFrameAlignment");
    factory_.registerNodeType<mercury_autonomy::GetCovariance>("getCovariance");
  }

  BT::BehaviorTreeFactory factory_;
};

TEST_F(LegacyActionNodeTest, RegistersAllLegacyNodes)
{
  const auto manifests = factory_.manifests();
  EXPECT_TRUE(manifests.count("GetFloat64Topic") > 0);
  EXPECT_TRUE(manifests.count("PublishInt8") > 0);
  EXPECT_TRUE(manifests.count("GetMappingState") > 0);
  EXPECT_TRUE(manifests.count("SetMappingTarget") > 0);
  EXPECT_TRUE(manifests.count("PublishEKFPose") > 0);
  EXPECT_TRUE(manifests.count("PublishToController") > 0);
  EXPECT_TRUE(manifests.count("SetControllerSafeMode") > 0);
  EXPECT_TRUE(manifests.count("SetStatus") > 0);
  EXPECT_TRUE(manifests.count("TriggerControllerStunt") > 0);
  EXPECT_TRUE(manifests.count("Wait") > 0);
  EXPECT_TRUE(manifests.count("WaitForDetection") > 0);
  EXPECT_TRUE(manifests.count("ComputeFrameAlignment") > 0);
  EXPECT_TRUE(manifests.count("getCovariance") > 0);
}

TEST_F(LegacyActionNodeTest, GetFloat64TopicPorts)
{
  const auto & ports = factory_.manifests().at("GetFloat64Topic").ports;
  EXPECT_TRUE(ports.count("topic") > 0);
  EXPECT_TRUE(ports.count("timeout") > 0);
  EXPECT_TRUE(ports.count("value") > 0);
}

TEST_F(LegacyActionNodeTest, PublishInt8Ports)
{
  const auto & ports = factory_.manifests().at("PublishInt8").ports;
  EXPECT_TRUE(ports.count("topic") > 0);
  EXPECT_TRUE(ports.count("data") > 0);
}

TEST_F(LegacyActionNodeTest, SetMappingTargetPorts)
{
  const auto & ports = factory_.manifests().at("SetMappingTarget").ports;
  EXPECT_TRUE(ports.count("target_object") > 0);
  EXPECT_TRUE(ports.count("lock_map") > 0);
  EXPECT_TRUE(ports.count("time_limit_secs") > 0);
}

TEST_F(LegacyActionNodeTest, PublishToControllerPorts)
{
  const auto & ports = factory_.manifests().at("PublishToController").ports;
  EXPECT_TRUE(ports.count("isOrientation") > 0);
  EXPECT_TRUE(ports.count("mode") > 0);
  EXPECT_TRUE(ports.count("x") > 0);
  EXPECT_TRUE(ports.count("y") > 0);
  EXPECT_TRUE(ports.count("z") > 0);
}

TEST_F(LegacyActionNodeTest, ComputeFrameAlignmentPorts)
{
  const auto & ports = factory_.manifests().at("ComputeFrameAlignment").ports;
  EXPECT_TRUE(ports.count("x") > 0);
  EXPECT_TRUE(ports.count("y") > 0);
  EXPECT_TRUE(ports.count("z") > 0);
  EXPECT_TRUE(ports.count("or") > 0);
  EXPECT_TRUE(ports.count("op") > 0);
  EXPECT_TRUE(ports.count("oy") > 0);
  EXPECT_TRUE(ports.count("reference_frame") > 0);
  EXPECT_TRUE(ports.count("link_frame") > 0);
  EXPECT_TRUE(ports.count("base_frame") > 0);
  EXPECT_TRUE(ports.count("out_x") > 0);
  EXPECT_TRUE(ports.count("out_y") > 0);
  EXPECT_TRUE(ports.count("out_z") > 0);
  EXPECT_TRUE(ports.count("out_or") > 0);
  EXPECT_TRUE(ports.count("out_op") > 0);
  EXPECT_TRUE(ports.count("out_oy") > 0);
}
