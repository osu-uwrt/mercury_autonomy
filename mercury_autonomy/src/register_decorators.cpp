// Register all custom decorator nodes with the BT.CPP factory.
// Add new decorator node registrations here when creating new decorators.

#include "mercury_autonomy/bt_decorators/example_decorator.hpp"
#include "mercury_autonomy/bt_decorators/retry_until_successful_or_timeout.hpp"

BT_REGISTER_NODES(factory) {
    factory.registerNodeType<mercury_autonomy::ExampleDecorator>("ExampleDecorator");
    factory.registerNodeType<mercury_autonomy::RetryUntilSuccessfulOrTimeout>(
        "RetryUntilSuccessfulOrTimeout");

    // Register additional decorator nodes below:
    // factory.registerNodeType<mercury_autonomy::MyNewDecorator>("MyNewDecorator");
}
