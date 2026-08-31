// Unit tests for PublishTwist and PublishBool action nodes.
// Verifies factory registration and port definitions.

#include <behaviortree_cpp/behavior_tree.h>
#include <behaviortree_cpp/bt_factory.h>
#include <gtest/gtest.h>

#include "mercury_autonomy/bt_actions/publish_bool.hpp"
#include "mercury_autonomy/bt_actions/publish_twist.hpp"

class PublishNodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        factory_.registerNodeType<mercury_autonomy::PublishTwist>("PublishTwist");
        factory_.registerNodeType<mercury_autonomy::PublishBool>("PublishBool");
    }

    BT::BehaviorTreeFactory factory_;
};

// ---------------------------------------------------------------------------
// PublishTwist tests
// ---------------------------------------------------------------------------

TEST_F(PublishNodeTest, PublishTwistRegisters) {
    auto manifests = factory_.manifests();
    ASSERT_TRUE(manifests.count("PublishTwist") > 0);
}

TEST_F(PublishNodeTest, PublishTwistHasCorrectPorts) {
    auto manifests = factory_.manifests();
    auto it = manifests.find("PublishTwist");
    ASSERT_NE(it, manifests.end());

    const auto & ports = it->second.ports;
    EXPECT_TRUE(ports.count("topic") > 0);
    EXPECT_TRUE(ports.count("linear_x") > 0);
    EXPECT_TRUE(ports.count("linear_y") > 0);
    EXPECT_TRUE(ports.count("linear_z") > 0);
    EXPECT_TRUE(ports.count("angular_x") > 0);
    EXPECT_TRUE(ports.count("angular_y") > 0);
    EXPECT_TRUE(ports.count("angular_z") > 0);
}

TEST_F(PublishNodeTest, PublishTwistPortCount) {
    auto manifests = factory_.manifests();
    auto it = manifests.find("PublishTwist");
    ASSERT_NE(it, manifests.end());
    // 7 input ports: topic + 6 velocity components
    EXPECT_EQ(it->second.ports.size(), 7u);
}

// ---------------------------------------------------------------------------
// PublishBool tests
// ---------------------------------------------------------------------------

TEST_F(PublishNodeTest, PublishBoolRegisters) {
    auto manifests = factory_.manifests();
    ASSERT_TRUE(manifests.count("PublishBool") > 0);
}

TEST_F(PublishNodeTest, PublishBoolHasCorrectPorts) {
    auto manifests = factory_.manifests();
    auto it = manifests.find("PublishBool");
    ASSERT_NE(it, manifests.end());

    const auto & ports = it->second.ports;
    EXPECT_TRUE(ports.count("topic") > 0);
    EXPECT_TRUE(ports.count("value") > 0);
}

TEST_F(PublishNodeTest, PublishBoolPortCount) {
    auto manifests = factory_.manifests();
    auto it = manifests.find("PublishBool");
    ASSERT_NE(it, manifests.end());
    // 2 input ports: topic + value
    EXPECT_EQ(it->second.ports.size(), 2u);
}
