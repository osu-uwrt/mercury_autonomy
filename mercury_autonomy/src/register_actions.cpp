// Register all custom action nodes with the BT.CPP factory.
// Add new action node registrations here when creating new actions.

#include "mercury_autonomy/bt_actions/example_action.hpp"
#include "mercury_autonomy/bt_actions/get_bool_topic.hpp"
#include "mercury_autonomy/bt_actions/get_float_topic.hpp"

// This macro exports the registration function for the shared library plugin.
BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<mercury_autonomy::ExampleAction>("ExampleAction");
  factory.registerNodeType<mercury_autonomy::GetBoolTopic>("GetBoolTopic");
  factory.registerNodeType<mercury_autonomy::GetFloatTopic>("GetFloatTopic");

  // Register additional action nodes below:
  // factory.registerNodeType<mercury_autonomy::MyNewAction>("MyNewAction");
}
