// Register all custom decorator nodes with the BT.CPP factory.
// Add new decorator node registrations here when creating new decorators.

#include "mercury_autonomy/bt_decorators/example_decorator.hpp"

BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<mercury_autonomy::ExampleDecorator>("ExampleDecorator");

  // Register additional decorator nodes below:
  // factory.registerNodeType<mercury_autonomy::MyNewDecorator>("MyNewDecorator");
}
