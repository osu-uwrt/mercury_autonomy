#!/usr/bin/env python3
"""
bt_assistant -- Automates creation and management of BT nodes for mercury_autonomy.

Usage::

    python3 scripts/bt_assistant.py create <type> <ClassName>
    python3 scripts/bt_assistant.py check
    python3 scripts/bt_assistant.py list

    python3 scripts/bt_assistant.py create action MoveToWaypoint
    python3 scripts/bt_assistant.py create condition IsSubmerged
    python3 scripts/bt_assistant.py create decorator RetryOnFail
"""

import argparse
import re
import sys
from pathlib import Path

# Resolve package root (mercury_autonomy/ directory containing CMakeLists.txt)
SCRIPT_DIR = Path(__file__).resolve().parent
PACKAGE_ROOT = SCRIPT_DIR.parent

INCLUDE_DIR = PACKAGE_ROOT / "include" / "mercury_autonomy"
SRC_DIR = PACKAGE_ROOT / "src"

NODE_TYPES = {
    "action": {
        "base_class": "MercuryActionNode",
        "bt_type": "bt_actions",
        "register_file": "register_actions.cpp",
        "header_template": "action_header",
        "source_template": "action_source",
    },
    "condition": {
        "base_class": "MercuryConditionNode",
        "bt_type": "bt_conditions",
        "register_file": "register_conditions.cpp",
        "header_template": "condition_header",
        "source_template": "condition_source",
    },
    "decorator": {
        "base_class": "MercuryDecoratorNode",
        "bt_type": "bt_decorators",
        "register_file": "register_decorators.cpp",
        "header_template": "decorator_header",
        "source_template": "decorator_source",
    },
}


def pascal_to_snake(name: str) -> str:
    """Convert PascalCase to snake_case."""
    s = re.sub(r"(?<=[a-z0-9])([A-Z])", r"_\1", name)
    s = re.sub(r"(?<=[A-Z])([A-Z][a-z])", r"_\1", s)
    return s.lower()


# Templates

TEMPLATES = {
    "action_header": '''\
// {class_name} -- custom BT action node.
//
// TODO: Add a description of what this action does.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy
{{

class {class_name} : public MercuryActionNode
{{
public:
  {class_name}(const std::string & name, const BT::NodeConfig & config)
  : MercuryActionNode(name, config) {{}}

  static BT::PortsList providedPorts()
  {{
    return {{
      // TODO: Define input/output ports.
      // BT::InputPort<std::string>("param", "Description"),
      // BT::OutputPort<double>("result", "Description"),
    }};
  }}

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

protected:
  void rosInit() override;
}};

}}  // namespace mercury_autonomy
''',
    "action_source": '''\
// {class_name} implementation.

#include "mercury_autonomy/{bt_type}/{file_name}.hpp"

namespace mercury_autonomy
{{

void {class_name}::rosInit()
{{
  // TODO: Create publishers, subscribers, service clients, etc.
  RCLCPP_DEBUG(rosNode()->get_logger(), "{class_name}::rosInit()");
}}

BT::NodeStatus {class_name}::onStart()
{{
  // TODO: Read input ports and begin action.
  RCLCPP_INFO(rosNode()->get_logger(), "{class_name} started.");
  return BT::NodeStatus::RUNNING;
}}

BT::NodeStatus {class_name}::onRunning()
{{
  // TODO: Check progress. Return RUNNING, SUCCESS, or FAILURE.
  return BT::NodeStatus::SUCCESS;
}}

void {class_name}::onHalted()
{{
  // TODO: Clean up resources.
  RCLCPP_INFO(rosNode()->get_logger(), "{class_name} halted.");
}}

}}  // namespace mercury_autonomy
''',
    "condition_header": '''\
// {class_name} -- custom BT condition node.
//
// TODO: Add a description of what this condition checks.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy
{{

class {class_name} : public MercuryConditionNode
{{
public:
  {class_name}(const std::string & name, const BT::NodeConfig & config)
  : MercuryConditionNode(name, config) {{}}

  static BT::PortsList providedPorts()
  {{
    return {{
      // TODO: Define input/output ports.
      // BT::InputPort<double>("value", "Description"),
    }};
  }}

  BT::NodeStatus tick() override;

protected:
  void rosInit() override;
}};

}}  // namespace mercury_autonomy
''',
    "condition_source": '''\
// {class_name} implementation.

#include "mercury_autonomy/{bt_type}/{file_name}.hpp"

namespace mercury_autonomy
{{

void {class_name}::rosInit()
{{
  // TODO: Create any ROS subscriptions needed to evaluate this condition.
  RCLCPP_DEBUG(rosNode()->get_logger(), "{class_name}::rosInit()");
}}

BT::NodeStatus {class_name}::tick()
{{
  // TODO: Evaluate the condition. Return SUCCESS or FAILURE.
  return BT::NodeStatus::SUCCESS;
}}

}}  // namespace mercury_autonomy
''',
    "decorator_header": '''\
// {class_name} -- custom BT decorator node.
//
// TODO: Add a description of what this decorator does.

#pragma once

#include "mercury_autonomy/autonomy_lib.hpp"

namespace mercury_autonomy
{{

class {class_name} : public MercuryDecoratorNode
{{
public:
  {class_name}(const std::string & name, const BT::NodeConfig & config)
  : MercuryDecoratorNode(name, config) {{}}

  static BT::PortsList providedPorts()
  {{
    return {{
      // TODO: Define input/output ports.
    }};
  }}

  BT::NodeStatus tick() override;

protected:
  void rosInit() override;
}};

}}  // namespace mercury_autonomy
''',
    "decorator_source": '''\
// {class_name} implementation.

#include "mercury_autonomy/{bt_type}/{file_name}.hpp"

namespace mercury_autonomy
{{

void {class_name}::rosInit()
{{
  RCLCPP_DEBUG(rosNode()->get_logger(), "{class_name}::rosInit()");
}}

BT::NodeStatus {class_name}::tick()
{{
  // TODO: Implement decorator logic. Tick child with:
  //   auto status = child_node_->executeTick();
  return child_node_->executeTick();
}}

}}  // namespace mercury_autonomy
''',
}


def create_node(node_type: str, class_name: str) -> None:
    """Create header, source, and registration entry for a new BT node."""
    if node_type not in NODE_TYPES:
        print(f"Error: Unknown node type '{node_type}'. Use: action, condition, decorator")
        sys.exit(1)

    # Validate class name is PascalCase
    if not re.match(r"^[A-Z][a-zA-Z0-9]+$", class_name):
        print(f"Error: Class name '{class_name}' must be PascalCase (e.g., MoveToWaypoint)")
        sys.exit(1)

    info = NODE_TYPES[node_type]
    file_name = pascal_to_snake(class_name)
    bt_type = info["bt_type"]

    header_dir = INCLUDE_DIR / bt_type
    source_dir = SRC_DIR / bt_type
    header_path = header_dir / f"{file_name}.hpp"
    source_path = source_dir / f"{file_name}.cpp"
    register_path = SRC_DIR / info["register_file"]

    # Check for existing files
    if header_path.exists():
        print(f"Error: Header already exists: {header_path}")
        sys.exit(1)
    if source_path.exists():
        print(f"Error: Source already exists: {source_path}")
        sys.exit(1)

    # Ensure directories exist
    header_dir.mkdir(parents=True, exist_ok=True)
    source_dir.mkdir(parents=True, exist_ok=True)

    # Format templates
    fmt = {"class_name": class_name, "file_name": file_name, "bt_type": bt_type}
    header_content = TEMPLATES[info["header_template"]].format(**fmt)
    source_content = TEMPLATES[info["source_template"]].format(**fmt)

    # Write files
    header_path.write_text(header_content)
    print(f"  Created header:  {header_path.relative_to(PACKAGE_ROOT)}")

    source_path.write_text(source_content)
    print(f"  Created source:  {source_path.relative_to(PACKAGE_ROOT)}")

    # Add registration entry
    _add_registration(register_path, class_name, file_name, bt_type)

    print(f"\n  Node '{class_name}' ({node_type}) created successfully.")
    print("  Remember to rebuild: colcon build --packages-select mercury_autonomy")


def _add_registration(register_path: Path, class_name: str, file_name: str, bt_type: str) -> None:
    """Add an #include and factory.registerNodeType line to the registration file."""
    content = register_path.read_text()

    include_line = f'#include "mercury_autonomy/{bt_type}/{file_name}.hpp"'
    register_line = f'  factory.registerNodeType<mercury_autonomy::{class_name}>("{class_name}");'

    if include_line in content:
        print(f"  Registration include already present in {register_path.name}")
        return

    # Add include after existing includes
    lines = content.split("\n")
    new_lines = []
    last_include_idx = -1

    for i, line in enumerate(lines):
        if line.startswith("#include"):
            last_include_idx = i

    # Insert include after last #include
    for i, line in enumerate(lines):
        new_lines.append(line)
        if i == last_include_idx:
            new_lines.append(include_line)

    content = "\n".join(new_lines)

    # Add registration before the closing brace of BT_REGISTER_NODES
    # Find the comment line "// Register additional..." and insert before it
    marker = "// Register additional"
    if marker in content:
        content = content.replace(marker, register_line + "\n\n  " + marker)
    else:
        # Fallback: insert before the last closing brace
        last_brace = content.rfind("}")
        content = content[:last_brace] + "  " + register_line + "\n" + content[last_brace:]

    register_path.write_text(content)
    print(f"  Updated registration: {register_path.relative_to(PACKAGE_ROOT)}")


def check_consistency() -> None:
    """Verify that all node source files have matching headers and registrations."""
    all_ok = True

    for node_type, info in NODE_TYPES.items():
        bt_type = info["bt_type"]
        source_dir = SRC_DIR / bt_type
        header_dir = INCLUDE_DIR / bt_type
        register_path = SRC_DIR / info["register_file"]
        register_content = register_path.read_text() if register_path.exists() else ""

        if not source_dir.exists():
            continue

        for src_file in sorted(source_dir.glob("*.cpp")):
            stem = src_file.stem
            header_file = header_dir / f"{stem}.hpp"

            # Check header exists
            if not header_file.exists():
                print(f"  WARN: {node_type} source '{src_file.name}' has no matching header")
                all_ok = False

            # Check if include line exists in register file
            include_line = f'#include "mercury_autonomy/{bt_type}/{stem}.hpp"'
            if include_line not in register_content:
                print(f"  WARN: {node_type} '{stem}' not included in {info['register_file']}")
                all_ok = False

    if all_ok:
        print("  All nodes are consistent: headers, sources, and registrations match.")
    else:
        print("\n  Some issues found. Run 'create' to generate missing files,")
        print("  or manually fix registration entries.")


def list_nodes() -> None:
    """List all registered BT nodes by type."""
    for node_type, info in NODE_TYPES.items():
        bt_type = info["bt_type"]
        source_dir = SRC_DIR / bt_type
        print(f"\n  {node_type.upper()} nodes ({bt_type}/):")

        if not source_dir.exists():
            print("    (none)")
            continue

        sources = sorted(source_dir.glob("*.cpp"))
        if not sources:
            print("    (none)")
        for src in sources:
            print(f"    - {src.stem}")


def main():
    parser = argparse.ArgumentParser(
        description="BT node assistant for mercury_autonomy",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    # create
    create_parser = subparsers.add_parser(
        "create", help="Create a new BT node (header, source, registration)")
    create_parser.add_argument(
        "type", choices=["action", "condition", "decorator"],
        help="The type of BT node to create")
    create_parser.add_argument(
        "name", help="PascalCase class name (e.g., MoveToWaypoint)")

    # check
    subparsers.add_parser(
        "check", help="Check consistency of headers, sources, and registrations")

    # list
    subparsers.add_parser(
        "list", help="List all existing BT nodes by type")

    args = parser.parse_args()

    print("\n  bt_assistant -- mercury_autonomy BT node helper\n")

    if args.command == "create":
        create_node(args.type, args.name)
    elif args.command == "check":
        check_consistency()
    elif args.command == "list":
        list_nodes()


if __name__ == "__main__":
    main()
