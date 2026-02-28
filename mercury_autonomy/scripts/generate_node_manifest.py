#!/usr/bin/env python3
"""
Generate a btstudio-compatible JSON node manifest from mercury_autonomy headers.

Parses the C++ header files in include/mercury_autonomy/bt_{actions,conditions,decorators}/
and extracts providedPorts() definitions to produce a JSON array of BTNodeDefinition objects
compatible with the btstudio nodeLibrary format.

Usage:
    python3 scripts/generate_node_manifest.py [--output node_manifest.json]

The output JSON can be imported into btstudio's src/data/nodeLibrary.ts or used
by external tooling to keep node definitions in sync with the C++ source.
"""

import argparse
import json
import os
import re
import sys

# Path from this script to the include root
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PACKAGE_DIR = os.path.dirname(SCRIPT_DIR)
INCLUDE_DIR = os.path.join(PACKAGE_DIR, "include", "mercury_autonomy")

# Map C++ port types to btstudio field types
CPP_TYPE_MAP = {
    "std::string": "string",
    "double": "number",
    "float": "number",
    "int": "number",
    "bool": "boolean",
}

# Subdirectories to scan and their btstudio categories
CATEGORIES = {
    "bt_actions": "action",
    "bt_conditions": "condition",
    "bt_decorators": "decorator",
}

# Skip example/template nodes
SKIP_PREFIXES = ("example_",)


def parse_port_line(line):
    """
    Parse a single BT::InputPort or BT::OutputPort declaration.

    Returns a dict with keys: name, type, valueType, value, description, portDirection.
    Returns None if the line does not contain a port declaration.
    """
    # Match: BT::InputPort<type>("name" ...) or BT::OutputPort<type>("name" ...)
    pattern = r'BT::(Input|Output)Port<([^>]+)>\s*\(\s*"([^"]+)"'
    match = re.search(pattern, line)
    if not match:
        return None

    direction_raw = match.group(1).lower()  # "input" or "output"
    cpp_type = match.group(2).strip()
    port_name = match.group(3)

    # Determine btstudio field type
    field_type = CPP_TYPE_MAP.get(cpp_type, "string")

    # Extract description (last quoted string in the line)
    all_strings = re.findall(r'"([^"]*)"', line)
    description = all_strings[-1] if len(all_strings) > 1 else ""

    # Extract default value (second argument, after the name)
    # Pattern: "name", default_value, "description"  or  "name", "description"
    default_value = _extract_default(line, field_type, len(all_strings))

    port_direction = "input" if direction_raw == "input" else "output"

    # Output ports use variable valueType and empty default
    if port_direction == "output":
        return {
            "name": port_name,
            "type": field_type,
            "valueType": "variable",
            "value": "",
            "description": description,
            "portDirection": "output",
        }

    return {
        "name": port_name,
        "type": field_type,
        "valueType": "literal",
        "value": default_value,
        "description": description,
        "portDirection": "input",
    }


def _extract_default(line, field_type, num_strings):
    """Extract default value from a port declaration line."""
    # Look for numeric default: "name", 3.0, "desc"
    num_match = re.search(
        r'"[^"]+"\s*,\s*(-?[\d.]+(?:e[+-]?\d+)?)\s*,', line
    )
    if num_match:
        val = num_match.group(1)
        if field_type == "number":
            return float(val) if "." in val else int(val)
        return val

    # Look for bool default: "name", true/false, "desc"
    bool_match = re.search(r'"[^"]+"\s*,\s*(true|false)\s*,', line)
    if bool_match:
        return bool_match.group(1) == "true"

    # Look for string default: "name", "default", "desc"  (3+ quoted strings)
    if num_strings >= 3:
        all_strings = re.findall(r'"([^"]*)"', line)
        return all_strings[1]

    # No default found -- use type-appropriate empty value
    if field_type == "number":
        return 0
    if field_type == "boolean":
        return False
    return ""


def parse_header(filepath, category):
    """Parse a single header file and return a BTNodeDefinition dict or None."""
    with open(filepath, "r") as f:
        content = f.read()

    # Extract class name
    class_match = re.search(r"class\s+(\w+)\s*:", content)
    if not class_match:
        return None

    class_name = class_match.group(1)

    # Extract description from top-of-file comment
    desc_match = re.match(r"//\s*(\w[^\n]+)", content)
    description = desc_match.group(1).strip() if desc_match else class_name

    # Remove trailing period if present
    if description.endswith("."):
        description = description[:-1]

    # Extract ports from providedPorts() block
    ports_match = re.search(
        r"providedPorts\s*\(\s*\)\s*\{(.*?)\}", content, re.DOTALL
    )
    if not ports_match:
        return None

    ports_block = ports_match.group(1)
    fields = []
    for line in ports_block.split("\n"):
        port = parse_port_line(line)
        if port:
            fields.append(port)

    # Build the id: snake_case from the BT registration name
    node_id = _to_kebab(class_name)

    return {
        "id": node_id,
        "type": class_name,
        "category": category,
        "name": _to_display_name(class_name),
        "description": description,
        "fields": fields,
    }


def _to_kebab(name):
    """Convert PascalCase to kebab-case id."""
    s = re.sub(r"([A-Z])", r"-\1", name).lstrip("-").lower()
    return s


def _to_display_name(name):
    """Convert PascalCase to spaced display name."""
    return re.sub(r"([a-z])([A-Z])", r"\1 \2", name)


def scan_all_nodes():
    """Scan all header directories and return list of BTNodeDefinition dicts."""
    nodes = []
    for subdir, category in CATEGORIES.items():
        dirpath = os.path.join(INCLUDE_DIR, subdir)
        if not os.path.isdir(dirpath):
            continue
        for filename in sorted(os.listdir(dirpath)):
            if not filename.endswith(".hpp"):
                continue
            # Skip example/template nodes
            if any(filename.startswith(p) for p in SKIP_PREFIXES):
                continue
            filepath = os.path.join(dirpath, filename)
            node_def = parse_header(filepath, category)
            if node_def:
                nodes.append(node_def)
    return nodes


def main():
    """Entry point for the manifest generator."""
    parser = argparse.ArgumentParser(
        description="Generate btstudio-compatible JSON node manifest."
    )
    parser.add_argument(
        "--output", "-o",
        default=os.path.join(PACKAGE_DIR, "node_manifest.json"),
        help="Output JSON file path (default: <package>/node_manifest.json)",
    )
    args = parser.parse_args()

    nodes = scan_all_nodes()
    if not nodes:
        print("WARNING: No nodes found. Check the include directory.", file=sys.stderr)
        sys.exit(1)

    manifest = {
        "_comment": "Auto-generated by generate_node_manifest.py. Do not edit manually.",
        "nodes": nodes,
    }

    with open(args.output, "w") as f:
        json.dump(manifest, f, indent=2)
        f.write("\n")

    print(f"Generated manifest with {len(nodes)} nodes -> {args.output}")


if __name__ == "__main__":
    main()
