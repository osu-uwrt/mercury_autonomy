// Unit tests for GetBoolTopic and GetFloatTopic action nodes.
// Verifies factory registration and port definitions.

#include <behaviortree_cpp/behavior_tree.h>
#include <behaviortree_cpp/bt_factory.h>
#include <gtest/gtest.h>

#include "mercury_autonomy/bt_actions/get_bool_topic.hpp"
#include "mercury_autonomy/bt_actions/get_float_topic.hpp"
#include "mercury_autonomy/bt_actions/wait_for_kill_switch.hpp"

class TopicNodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        factory_.registerNodeType<mercury_autonomy::GetBoolTopic>("GetBoolTopic");
        factory_.registerNodeType<mercury_autonomy::GetFloatTopic>("GetFloatTopic");
        factory_.registerNodeType<mercury_autonomy::WaitForKillSwitch>("WaitForKillSwitch");
    }

    BT::BehaviorTreeFactory factory_;
};

TEST_F(TopicNodeTest, GetBoolTopicRegisters) {
    auto manifests = factory_.manifests();
    ASSERT_TRUE(manifests.count("GetBoolTopic") > 0);
}

TEST_F(TopicNodeTest, GetBoolTopicHasCorrectPorts) {
    auto manifests = factory_.manifests();
    auto it = manifests.find("GetBoolTopic");
    ASSERT_NE(it, manifests.end());

    const auto & ports = it->second.ports;
    EXPECT_TRUE(ports.count("topic") > 0);
    EXPECT_TRUE(ports.count("value") > 0);
    EXPECT_TRUE(ports.count("timeout") > 0);
}

TEST_F(TopicNodeTest, GetFloatTopicRegisters) {
    auto manifests = factory_.manifests();
    ASSERT_TRUE(manifests.count("GetFloatTopic") > 0);
}

TEST_F(TopicNodeTest, GetFloatTopicHasCorrectPorts) {
    auto manifests = factory_.manifests();
    auto it = manifests.find("GetFloatTopic");
    ASSERT_NE(it, manifests.end());

    const auto & ports = it->second.ports;
    EXPECT_TRUE(ports.count("topic") > 0);
    EXPECT_TRUE(ports.count("value") > 0);
    EXPECT_TRUE(ports.count("timeout") > 0);
}

TEST_F(TopicNodeTest, WaitForKillSwitchRegisters) {
    auto manifests = factory_.manifests();
    ASSERT_TRUE(manifests.count("WaitForKillSwitch") > 0);
}

TEST_F(TopicNodeTest, WaitForKillSwitchHasCorrectPorts) {
    auto manifests = factory_.manifests();
    auto it = manifests.find("WaitForKillSwitch");
    ASSERT_NE(it, manifests.end());

    const auto & ports = it->second.ports;
    EXPECT_TRUE(ports.count("topic") > 0);
}
