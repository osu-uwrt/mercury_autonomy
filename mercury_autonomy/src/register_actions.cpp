// Register all custom action nodes with the BT.CPP factory.
// Add new action node registrations here when creating new actions.

#include "mercury_autonomy/bt_actions/example_action.hpp"

// This macro exports the registration function for the shared library plugin.
BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<mercury_autonomy::ExampleAction>("ExampleAction");

  // Register additional action nodes below:
  // factory.registerNodeType<mercury_autonomy::MyNewAction>("MyNewAction");
}
