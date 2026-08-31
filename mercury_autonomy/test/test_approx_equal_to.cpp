// Unit tests for the ApproxEqualTo condition node.

#include <behaviortree_cpp/behavior_tree.h>
#include <behaviortree_cpp/bt_factory.h>
#include <gtest/gtest.h>

#include "mercury_autonomy/bt_conditions/approx_equal_to.hpp"

class ApproxEqualToTest : public ::testing::Test {
protected:
    void SetUp() override {
        factory_.registerNodeType<mercury_autonomy::ApproxEqualTo>("ApproxEqualTo");
    }

    BT::NodeStatus runTree(double a, double b, double range) {
        std::string xml =
            R"(
      <root BTCPP_format="4">
        <BehaviorTree ID="TestApprox">
          <ApproxEqualTo a=")" +
            std::to_string(a) + R"(" b=")" + std::to_string(b) + R"(" range=")" +
            std::to_string(range) + R"(" />
        </BehaviorTree>
      </root>
    )";

        BT::Tree tree = factory_.createTreeFromText(xml);

        // ApproxEqualTo inherits MercuryBtNode which needs a ROS node.
        // For unit testing without ROS, we test the factory registration works.
        // A full integration test would use rclcpp::init.
        return tree.tickWhileRunning();
    }

    BT::BehaviorTreeFactory factory_;
};

TEST_F(ApproxEqualToTest, RegistersWithFactory) {
    // Check that the node type exists in the factory manifest
    auto manifests = factory_.manifests();
    ASSERT_TRUE(manifests.count("ApproxEqualTo") > 0);
}

TEST_F(ApproxEqualToTest, HasCorrectPorts) {
    auto manifests = factory_.manifests();
    auto it = manifests.find("ApproxEqualTo");
    ASSERT_NE(it, manifests.end());

    const auto & ports = it->second.ports;
    EXPECT_TRUE(ports.count("a") > 0);
    EXPECT_TRUE(ports.count("b") > 0);
    EXPECT_TRUE(ports.count("range") > 0);
}
