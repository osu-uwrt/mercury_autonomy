// Unit tests for the RetryUntilSuccessfulOrTimeout decorator node.
// Verifies factory registration and port definitions.

#include <behaviortree_cpp/behavior_tree.h>
#include <behaviortree_cpp/bt_factory.h>
#include <gtest/gtest.h>

#include "mercury_autonomy/bt_decorators/retry_until_successful_or_timeout.hpp"

class DecoratorNodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        factory_.registerNodeType<mercury_autonomy::RetryUntilSuccessfulOrTimeout>(
            "RetryUntilSuccessfulOrTimeout");
    }

    BT::BehaviorTreeFactory factory_;
};

TEST_F(DecoratorNodeTest, RetryUntilSuccessfulOrTimeoutRegisters) {
    auto manifests = factory_.manifests();
    ASSERT_TRUE(manifests.count("RetryUntilSuccessfulOrTimeout") > 0);
}

TEST_F(DecoratorNodeTest, RetryUntilSuccessfulOrTimeoutHasCorrectPorts) {
    auto manifests = factory_.manifests();
    auto it = manifests.find("RetryUntilSuccessfulOrTimeout");
    ASSERT_NE(it, manifests.end());

    const auto & ports = it->second.ports;
    EXPECT_TRUE(ports.count("timeout") > 0);
}
