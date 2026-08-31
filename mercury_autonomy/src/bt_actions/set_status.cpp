// SetStatus -- publishes a LED status message for the current tree state.

#include "mercury_autonomy/bt_actions/set_status.hpp"

#include <map>

namespace mercury_autonomy {

namespace {
constexpr char kLedTopic[] = "command/led";

enum class TreeStatus {
    Undefined,
    Waiting,
    Starting,
    Moving,
    Searching,
    Aligning,
    Performing,
    Success,
    Failure,
    Panic,
};

const std::map<std::string, TreeStatus> kStatuses = {
    {"undefined", TreeStatus::Undefined},   {"waiting", TreeStatus::Waiting},
    {"starting", TreeStatus::Starting},     {"moving", TreeStatus::Moving},
    {"searching", TreeStatus::Searching},   {"aligning", TreeStatus::Aligning},
    {"performing", TreeStatus::Performing}, {"success", TreeStatus::Success},
    {"failure", TreeStatus::Failure},       {"panic", TreeStatus::Panic},
};
}  // namespace

void SetStatus::rosInit() {
    pub_ = rosNode()->create_publisher<mercury_msgs::msg::LedCommand>(kLedTopic, 10);
}

BT::NodeStatus SetStatus::onStart() {
    std::string status_name = "undefined";
    getInput("status", status_name);
    if (!kStatuses.count(status_name)) {
        RCLCPP_WARN(rosNode()->get_logger(), "SetStatus: unknown status '%s'", status_name.c_str());
        status_name = "undefined";
    }

    mercury_msgs::msg::LedCommand cmd;
    switch (kStatuses.at(status_name)) {
        case TreeStatus::Undefined:
            cmd.red = 255;
            cmd.green = 255;
            cmd.blue = 255;
            cmd.mode = cmd.MODE_SOLID;
            cmd.target = cmd.TARGET_ALL;
            break;
        case TreeStatus::Waiting:
            cmd.red = 255;
            cmd.green = 0;
            cmd.blue = 0;
            cmd.mode = cmd.MODE_SOLID;
            cmd.target = cmd.TARGET_ALL;
            break;
        case TreeStatus::Starting:
            cmd.red = 0;
            cmd.green = 0;
            cmd.blue = 255;
            cmd.mode = cmd.MODE_SLOW_FLASH;
            cmd.target = cmd.TARGET_ALL;
            break;
        case TreeStatus::Moving:
            cmd.red = 0;
            cmd.green = 255;
            cmd.blue = 0;
            cmd.mode = cmd.MODE_SOLID;
            cmd.target = cmd.TARGET_ALL;
            break;
        case TreeStatus::Searching:
            cmd.red = 255;
            cmd.green = 100;
            cmd.blue = 0;
            cmd.mode = cmd.MODE_SOLID;
            cmd.target = cmd.TARGET_ALL;
            break;
        case TreeStatus::Aligning:
            cmd.red = 255;
            cmd.green = 0;
            cmd.blue = 255;
            cmd.mode = cmd.MODE_SLOW_FLASH;
            cmd.target = cmd.TARGET_ALL;
            break;
        case TreeStatus::Performing:
            cmd.red = 0;
            cmd.green = 0;
            cmd.blue = 255;
            cmd.mode = cmd.MODE_FAST_FLASH;
            cmd.target = cmd.TARGET_ALL;
            break;
        case TreeStatus::Success:
            cmd.red = 0;
            cmd.green = 255;
            cmd.blue = 0;
            cmd.mode = cmd.MODE_BREATH;
            cmd.target = cmd.TARGET_ALL;
            break;
        case TreeStatus::Failure:
            cmd.red = 255;
            cmd.green = 0;
            cmd.blue = 0;
            cmd.mode = cmd.MODE_BREATH;
            cmd.target = cmd.TARGET_ALL;
            break;
        case TreeStatus::Panic:
            cmd.red = 255;
            cmd.green = 0;
            cmd.blue = 0;
            cmd.mode = cmd.MODE_FAST_FLASH;
            cmd.target = cmd.TARGET_ALL;
            break;
    }

    pub_->publish(cmd);
    return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus SetStatus::onRunning() { return BT::NodeStatus::SUCCESS; }

void SetStatus::onHalted() {}

}  // namespace mercury_autonomy
