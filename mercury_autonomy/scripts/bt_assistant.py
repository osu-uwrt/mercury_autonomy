#!/usr/bin/env python3
"""
bt_assistant -- Automates creation and management of BT nodes for mercury_autonomy.

Templates are sourced from the example node files (example_action, example_condition,
example_decorator) so that editing those files automatically updates what new nodes
receive. The leading comment block is replaced with a generic one for new nodes.

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
        "example_class": "ExampleAction",
    },
    "condition": {
        "base_class": "MercuryConditionNode",
        "bt_type": "bt_conditions",
        "register_file": "register_conditions.cpp",
        "example_class": "ExampleCondition",
    },
    "decorator": {
        "base_class": "MercuryDecoratorNode",
        "bt_type": "bt_decorators",
        "register_file": "register_decorators.cpp",
        "example_class": "ExampleDecorator",
    },
}


def pascal_to_snake(name: str) -> str:
    """Convert PascalCase to snake_case."""
    s = re.sub(r"(?<=[a-z0-9])([A-Z])", r"_\1", name)
    s = re.sub(r"(?<=[A-Z])([A-Z][a-z])", r"_\1", s)
    return s.lower()


def _read_example_file(node_type: str, file_kind: str) -> str:
    """
    Read an example template file and return its contents.

    :param node_type: One of 'action', 'condition', 'decorator'.
    :param file_kind: Either 'header' or 'source'.
    :return: File contents as a string.
    """
    info = NODE_TYPES[node_type]
    bt_type = info["bt_type"]
    example_file = pascal_to_snake(info["example_class"])

    if file_kind == "header":
        path = INCLUDE_DIR / bt_type / f"{example_file}.hpp"
    else:
        path = SRC_DIR / bt_type / f"{example_file}.cpp"

    if not path.exists():
        print(f"Error: Template file not found: {path}")
        print("  Ensure the example node files exist before creating new nodes.")
        sys.exit(1)

    return path.read_text()


def _replace_leading_comment(content: str, new_comment: str, marker: str) -> str:
    """
    Replace everything before a marker line with a new comment block.

    :param content: Full file text.
    :param new_comment: Replacement comment text (should end with newline).
    :param marker: Line that marks end of the comment block (e.g. '#pragma once').
    :return: Content with the leading comment replaced.
    """
    idx = content.find(marker)
    if idx == -1:
        return new_comment + content
    return new_comment + content[idx:]


def _build_from_example(node_type: str, file_kind: str,
                        class_name: str, file_name: str) -> str:
    """
    Build new node file content by transforming the example template.

    Reads the example file, replaces the leading comment with a generic one,
    and substitutes the example class/file names with the new ones.
    """
    info = NODE_TYPES[node_type]
    example_class = info["example_class"]
    example_file = pascal_to_snake(example_class)
    content = _read_example_file(node_type, file_kind)

    # Replace leading comment block
    type_label = node_type  # "action", "condition", or "decorator"
    if file_kind == "header":
        new_comment = (
            f"// {class_name} -- custom BT {type_label} node.\n"
            f"//\n"
            f"// TODO: Add a description of what this {type_label} does.\n\n"
        )
        content = _replace_leading_comment(content, new_comment, "#pragma once")
    else:
        new_comment = f"// {class_name} implementation.\n\n"
        content = _replace_leading_comment(content, new_comment, "#include")

    # Substitute class and file names
    content = content.replace(example_class, class_name)
    content = content.replace(example_file, file_name)

    return content


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

    # Build content from example template files
    header_content = _build_from_example(node_type, "header", class_name, file_name)
    source_content = _build_from_example(node_type, "source", class_name, file_name)

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
