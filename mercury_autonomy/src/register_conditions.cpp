// Register all custom condition nodes with the BT.CPP factory.
// Add new condition node registrations here when creating new conditions.

#include "mercury_autonomy/bt_conditions/example_condition.hpp"
#include "mercury_autonomy/bt_conditions/approx_equal_to.hpp"

BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<mercury_autonomy::ExampleCondition>("ExampleCondition");
  factory.registerNodeType<mercury_autonomy::ApproxEqualTo>("ApproxEqualTo");

  // Register additional condition nodes below:
  // factory.registerNodeType<mercury_autonomy::MyNewCondition>("MyNewCondition");
}
