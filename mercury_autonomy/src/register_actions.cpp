// Register all custom action nodes with the BT.CPP factory.
// Add new action node registrations here when creating new actions.

#include "mercury_autonomy/bt_actions/example_action.hpp"
#include "mercury_autonomy/bt_actions/get_bool_topic.hpp"
#include "mercury_autonomy/bt_actions/get_float_topic.hpp"
#include "mercury_autonomy/bt_actions/call_set_bool_service.hpp"
#include "mercury_autonomy/bt_actions/call_trigger_service.hpp"
#include "mercury_autonomy/bt_actions/transform_pose.hpp"
#include "mercury_autonomy/bt_actions/get_odometry.hpp"
#include "mercury_autonomy/bt_actions/get_imu_orientation.hpp"
#include "mercury_autonomy/bt_actions/get_twist_topic.hpp"
#include "mercury_autonomy/bt_actions/publish_twist.hpp"
#include "mercury_autonomy/bt_actions/publish_bool.hpp"

// This macro exports the registration function for the shared library plugin.
BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<mercury_autonomy::ExampleAction>("ExampleAction");
  factory.registerNodeType<mercury_autonomy::GetBoolTopic>("GetBoolTopic");
  factory.registerNodeType<mercury_autonomy::GetFloatTopic>("GetFloatTopic");
  factory.registerNodeType<mercury_autonomy::CallSetBoolService>("CallSetBoolService");
  factory.registerNodeType<mercury_autonomy::CallTriggerService>("CallTriggerService");
  factory.registerNodeType<mercury_autonomy::TransformPose>("TransformPose");
  factory.registerNodeType<mercury_autonomy::GetOdometry>("GetOdometry");
  factory.registerNodeType<mercury_autonomy::GetImuOrientation>("GetImuOrientation");
  factory.registerNodeType<mercury_autonomy::GetTwistTopic>("GetTwistTopic");
  factory.registerNodeType<mercury_autonomy::PublishTwist>("PublishTwist");
  factory.registerNodeType<mercury_autonomy::PublishBool>("PublishBool");

  // Register additional action nodes below:
  // factory.registerNodeType<mercury_autonomy::MyNewAction>("MyNewAction");
}
