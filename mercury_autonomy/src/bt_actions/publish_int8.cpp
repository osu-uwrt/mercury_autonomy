// PublishInt8 -- publishes an integer command topic and returns SUCCESS.

#include "mercury_autonomy/bt_actions/publish_int8.hpp"

namespace mercury_autonomy {

void PublishInt8::rosInit() {}

BT::NodeStatus PublishInt8::onStart() {
    auto topic = getInput<std::string>("topic");
    if (!topic) {
        RCLCPP_ERROR(rosNode()->get_logger(), "PublishInt8: missing required input [topic]");
        return BT::NodeStatus::FAILURE;
    }

    if (!pub_ || topic.value() != last_topic_) {
        pub_ = rosNode()->create_publisher<std_msgs::msg::UInt16>(topic.value(), 10);
        last_topic_ = topic.value();
    }

    int data = 0;
    getInput("data", data);

    std_msgs::msg::UInt16 msg;
    msg.data = static_cast<uint16_t>(data);
    pub_->publish(msg);

    return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus PublishInt8::onRunning() { return BT::NodeStatus::SUCCESS; }

void PublishInt8::onHalted() {}

}  // namespace mercury_autonomy
