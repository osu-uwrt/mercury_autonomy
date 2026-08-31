// WaitForDetection -- waits until a detection with the requested class id arrives.

#include "mercury_autonomy/bt_actions/wait_for_detection.hpp"

namespace mercury_autonomy {

namespace {
constexpr char kDetectionsTopic[] = "detected_objects";
}

void WaitForDetection::rosInit() {
    sub_ = rosNode()->create_subscription<vision_msgs::msg::Detection3DArray>(
        kDetectionsTopic, 10,
        [this](vision_msgs::msg::Detection3DArray::SharedPtr msg) { detectionCallback(msg); });
}

BT::NodeStatus WaitForDetection::onStart() {
    getInput("object_name", target_object_id_);
    getInput("timeout_secs", timeout_secs_);
    found_object_ = false;
    start_time_ = rosNode()->now();
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus WaitForDetection::onRunning() {
    if (found_object_) {
        return BT::NodeStatus::SUCCESS;
    }

    if ((rosNode()->now() - start_time_).seconds() > timeout_secs_) {
        return BT::NodeStatus::FAILURE;
    }

    return BT::NodeStatus::RUNNING;
}

void WaitForDetection::onHalted() {}

void WaitForDetection::detectionCallback(const vision_msgs::msg::Detection3DArray::SharedPtr msg) {
    for (const auto & detection : msg->detections) {
        if (!detection.results.empty() &&
            detection.results.front().hypothesis.class_id == target_object_id_) {
            found_object_ = true;
            return;
        }
    }
}

} // namespace mercury_autonomy
