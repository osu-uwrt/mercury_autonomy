// Register all custom condition nodes with the BT.CPP factory.
// Add new condition node registrations here when creating new conditions.

#include "mercury_autonomy/bt_conditions/example_condition.hpp"

BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<mercury_autonomy::ExampleCondition>("ExampleCondition");

  // Register additional condition nodes below:
  // factory.registerNodeType<mercury_autonomy::MyNewCondition>("MyNewCondition");
}
