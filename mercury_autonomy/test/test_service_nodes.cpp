// Unit tests for service-calling action nodes.
// Verifies factory registration and port definitions for
// CallSetBoolService and CallTriggerService.

#include <gtest/gtest.h>

#include <behaviortree_cpp/behavior_tree.h>
#include <behaviortree_cpp/bt_factory.h>

#include "mercury_autonomy/bt_actions/call_set_bool_service.hpp"
#include "mercury_autonomy/bt_actions/call_trigger_service.hpp"

class ServiceNodeTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    factory_.registerNodeType<mercury_autonomy::CallSetBoolService>("CallSetBoolService");
    factory_.registerNodeType<mercury_autonomy::CallTriggerService>("CallTriggerService");
  }

  BT::BehaviorTreeFactory factory_;
};

TEST_F(ServiceNodeTest, CallSetBoolServiceRegisters)
{
  auto manifests = factory_.manifests();
  ASSERT_TRUE(manifests.count("CallSetBoolService") > 0);
}

TEST_F(ServiceNodeTest, CallSetBoolServiceHasCorrectPorts)
{
  auto manifests = factory_.manifests();
  auto it = manifests.find("CallSetBoolService");
  ASSERT_NE(it, manifests.end());

  const auto & ports = it->second.ports;
  EXPECT_TRUE(ports.count("srv_name") > 0);
  EXPECT_TRUE(ports.count("data") > 0);
  EXPECT_TRUE(ports.count("timeout") > 0);
}

TEST_F(ServiceNodeTest, CallTriggerServiceRegisters)
{
  auto manifests = factory_.manifests();
  ASSERT_TRUE(manifests.count("CallTriggerService") > 0);
}

TEST_F(ServiceNodeTest, CallTriggerServiceHasCorrectPorts)
{
  auto manifests = factory_.manifests();
  auto it = manifests.find("CallTriggerService");
  ASSERT_NE(it, manifests.end());

  const auto & ports = it->second.ports;
  EXPECT_TRUE(ports.count("srv_name") > 0);
  EXPECT_TRUE(ports.count("timeout") > 0);
}
