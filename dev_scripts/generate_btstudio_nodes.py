#!/usr/bin/env python3
"""
Generate btstudio-compatible TypeScript and JSON node definitions from C++ headers.

Parses the BT::InputPort/OutputPort declarations in mercury_autonomy headers and
emits both a JSON manifest and a TypeScript file importable by btstudio.

Usage:
    python3 dev_scripts/generate_btstudio_nodes.py
    python3 dev_scripts/generate_btstudio_nodes.py --json-only
    python3 dev_scripts/generate_btstudio_nodes.py --ts-only
    python3 dev_scripts/generate_btstudio_nodes.py -o custom_output_dir/
"""

import argparse
import json
import os
import re
import sys

# Path resolution -- this script lives in <repo_root>/dev_scripts/
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.dirname(SCRIPT_DIR)
PACKAGE_DIR = os.path.join(REPO_ROOT, "mercury_autonomy")
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

# Skip example/template nodes (not useful for btstudio users)
SKIP_PREFIXES = ("example_",)


def parse_port_line(line):
    """
    Parse a single BT::InputPort or BT::OutputPort declaration.

    Returns a dict with btstudio NodeField keys, or None if the line
    does not contain a port declaration.
    """
    pattern = r'BT::(Input|Output)Port<([^>]+)>\s*\(\s*"([^"]+)"'
    match = re.search(pattern, line)
    if not match:
        return None

    direction_raw = match.group(1).lower()
    cpp_type = match.group(2).strip()
    port_name = match.group(3)

    field_type = CPP_TYPE_MAP.get(cpp_type, "string")

    # Extract description (last quoted string in the line)
    all_strings = re.findall(r'"([^"]*)"', line)
    description = all_strings[-1] if len(all_strings) > 1 else ""

    default_value = _extract_default(line, field_type, len(all_strings))
    port_direction = "input" if direction_raw == "input" else "output"

    # Output ports always use variable valueType
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
    """Extract the default value from a port declaration, if present."""
    # Numeric default: "name", 3.0, "desc"
    num_match = re.search(
        r'"[^"]+"\s*,\s*(-?[\d.]+(?:e[+-]?\d+)?)\s*,', line
    )
    if num_match:
        val = num_match.group(1)
        if field_type == "number":
            return float(val) if "." in val else int(val)
        return val

    # Boolean default: "name", true/false, "desc"
    bool_match = re.search(r'"[^"]+"\s*,\s*(true|false)\s*,', line)
    if bool_match:
        return bool_match.group(1) == "true"

    # String default: "name", "default", "desc" (3+ quoted strings)
    if num_strings >= 3:
        all_strings = re.findall(r'"([^"]*)"', line)
        return all_strings[1]

    # No default -- type-appropriate empty value
    if field_type == "number":
        return 0
    if field_type == "boolean":
        return False
    return ""


def parse_header(filepath, category):
    """Parse a single C++ header and return a BTNodeDefinition dict or None."""
    with open(filepath, "r") as f:
        content = f.read()

    class_match = re.search(r"class\s+(\w+)\s*:", content)
    if not class_match:
        return None

    class_name = class_match.group(1)

    # First-line comment becomes the description
    desc_match = re.match(r"//\s*(\w[^\n]+)", content)
    description = desc_match.group(1).strip() if desc_match else class_name
    if description.endswith("."):
        description = description[:-1]

    # Extract ports from providedPorts()
    ports_match = re.search(
        r"providedPorts\s*\(\s*\)\s*\{(.*?)\}", content, re.DOTALL
    )
    if not ports_match:
        return None

    fields = []
    for line in ports_match.group(1).split("\n"):
        port = parse_port_line(line)
        if port:
            fields.append(port)

    return {
        "id": _to_kebab(class_name),
        "type": class_name,
        "category": category,
        "name": _to_display_name(class_name),
        "description": description,
        "fields": fields,
    }


def _to_kebab(name):
    """Convert PascalCase to kebab-case with 'mercury-' prefix."""
    s = re.sub(r"([A-Z])", r"-\1", name).lstrip("-").lower()
    return "mercury-" + s


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
            if any(filename.startswith(p) for p in SKIP_PREFIXES):
                continue
            filepath = os.path.join(dirpath, filename)
            node_def = parse_header(filepath, category)
            if node_def:
                nodes.append(node_def)
    return nodes


def generate_json(nodes, output_path):
    """Write the JSON manifest file."""
    manifest = {
        "_comment": "Auto-generated by generate_btstudio_nodes.py. Do not edit.",
        "nodes": nodes,
    }
    with open(output_path, "w") as f:
        json.dump(manifest, f, indent=2)
        f.write("\n")
    print(f"  JSON manifest: {output_path} ({len(nodes)} nodes)")


def _format_ts_value(value):
    """Format a Python value as a TypeScript literal."""
    if isinstance(value, bool):
        return "true" if value else "false"
    if isinstance(value, (int, float)):
        return str(value)
    # String -- wrap in single quotes
    return f"'{value}'"


def generate_typescript(nodes, output_path):
    """Write the TypeScript node library file for btstudio."""
    lines = [
        "// Auto-generated by generate_btstudio_nodes.py -- do not edit manually.",
        "//",
        "// To regenerate:  python3 dev_scripts/generate_btstudio_nodes.py",
        "//",
        "// Import this file in btstudio's nodeLibrary.ts:",
        "//   import { mercuryAutonomyNodes } from './mercury_autonomy_nodes';",
        "//   export const nodeLibrary = [...existingNodes, ...mercuryAutonomyNodes];",
        "",
        "import type { BTNodeDefinition } from '../types';",
        "",
        "export const mercuryAutonomyNodes: BTNodeDefinition[] = [",
    ]

    # Group by category for readable output
    by_category = {}
    for node in nodes:
        by_category.setdefault(node["category"], []).append(node)

    first_node = True
    for category in ["action", "condition", "decorator"]:
        cat_nodes = by_category.get(category, [])
        if not cat_nodes:
            continue

        lines.append(f"  // -- {category.upper()}S " + "-" * (68 - len(category)))

        for node in cat_nodes:
            if not first_node:
                lines.append("")
            first_node = False

            lines.append("  {")
            lines.append(f"    id: '{node['id']}',")
            lines.append(f"    type: '{node['type']}',")
            lines.append(f"    category: '{node['category']}',")
            lines.append(f"    name: '{node['name']}',")
            # Escape single quotes in description
            desc = node["description"].replace("'", "\\'")
            lines.append(f"    description: '{desc}',")

            if node["fields"]:
                lines.append("    fields: [")
                for field in node["fields"]:
                    val = _format_ts_value(field["value"])
                    fd = field.get("description", "").replace("'", "\\'")
                    lines.append(
                        f"      {{ name: '{field['name']}', type: '{field['type']}', "
                        f"valueType: '{field['valueType']}', value: {val}, "
                        f"description: '{fd}', portDirection: '{field['portDirection']}' }},"
                    )
                lines.append("    ],")
            else:
                lines.append("    fields: [],")

            lines.append("  },")

    lines.append("];")
    lines.append("")

    with open(output_path, "w") as f:
        f.write("\n".join(lines))
    print(f"  TypeScript:    {output_path} ({len(nodes)} nodes)")


def main():
    parser = argparse.ArgumentParser(
        description="Generate btstudio node definitions from mercury_autonomy C++ headers.",
    )
    parser.add_argument(
        "-o", "--output-dir",
        default=os.path.join(REPO_ROOT, "dev_scripts"),
        help="Directory for generated files (default: dev_scripts/)",
    )
    parser.add_argument(
        "--json-only", action="store_true",
        help="Only generate JSON manifest",
    )
    parser.add_argument(
        "--ts-only", action="store_true",
        help="Only generate TypeScript file",
    )
    args = parser.parse_args()

    nodes = scan_all_nodes()
    if not nodes:
        print("ERROR: No nodes found. Check the include directory.", file=sys.stderr)
        sys.exit(1)

    os.makedirs(args.output_dir, exist_ok=True)
    print(f"\n  Scanned {len(nodes)} nodes from {INCLUDE_DIR}\n")

    if not args.ts_only:
        json_path = os.path.join(args.output_dir, "node_manifest.json")
        generate_json(nodes, json_path)

    if not args.json_only:
        ts_path = os.path.join(args.output_dir, "mercury_autonomy_nodes.ts")
        generate_typescript(nodes, ts_path)

    print("\n  Done.\n")


if __name__ == "__main__":
    main()
