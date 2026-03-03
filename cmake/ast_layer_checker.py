# cmake/ast_layer_checker.py

import sys
import re
from pathlib import Path
from collections import defaultdict

# ============================================================
# ANSI Colors (Auto-disable in CI if needed)
# ============================================================

USE_COLOR = sys.stdout.isatty()


def color(code):
    return f"\033[{code}m" if USE_COLOR else ""


RED = color("31")
GREEN = color("32")
YELLOW = color("33")
CYAN = color("36")
RESET = color("0")

# ============================================================
# Project Setup
# ============================================================

if len(sys.argv) != 2:
    print("Usage: python ast_layer_checker.py <project_root>")
    sys.exit(2)

PROJECT_ROOT = Path(sys.argv[1]).resolve()

SRC_DIR = PROJECT_ROOT / "src"
INCLUDE_DIR = PROJECT_ROOT / "include"

if not SRC_DIR.exists():
    print(f"{RED}ERROR:{RESET} src/ directory not found.")
    sys.exit(2)

# ============================================================
# Layer Rules
# ============================================================

LAYER_RULES = {
    "core": {
        "allowed": {"core"},
        "forbidden": {"system", "platform"},
    },
    "system": {
        "allowed": {"core", "system", "platform"},
        "forbidden": set(),
    },
    "platform": {
        # "allowed": set(),
        "allowed": {"platform"},
        "forbidden": {"core", "system"},
    },
}

# Layer dependency graph (layer -> set(layers))
layer_graph = defaultdict(set)

violations = []
files_scanned = 0

# ============================================================
# Layer Detection
# ============================================================


def detect_layer(path: Path):
    parts = path.parts
    if "core" in parts:
        return "core"
    if "system" in parts:
        return "system"
    if "platform" in parts:
        return "platform"
    return None


# ============================================================
# Include Parsing (Deterministic, No Clang Required)
# ============================================================

INCLUDE_PATTERN = re.compile(r'#include\s+"([^"]+)"')


def parse_includes(file_path: Path):
    try:
        content = file_path.read_text(encoding="utf-8", errors="ignore")
    except Exception:
        return []

    return INCLUDE_PATTERN.findall(content)


# ============================================================
# File Checking
# ============================================================


def check_file(file_path: Path):
    global files_scanned
    files_scanned += 1

    layer = detect_layer(file_path)
    if not layer:
        return

    includes = parse_includes(file_path)

    for inc in includes:
        if not inc.startswith("cleaner/"):
            continue

        target_layer = None
        if inc.startswith("cleaner/core/"):
            target_layer = "core"
        elif inc.startswith("cleaner/system/"):
            target_layer = "system"
        elif inc.startswith("cleaner/platform/"):
            target_layer = "platform"

        if not target_layer:
            continue

        # Build layer graph
        layer_graph[layer].add(target_layer)

        # Enforce rule
        if target_layer not in LAYER_RULES[layer]["allowed"]:
            violations.append(f"{layer} -> {target_layer} in {file_path}")


# ============================================================
# Cycle Detection (Layer-Level)
# ============================================================


def detect_cycles():
    visited = set()
    stack = set()

    def dfs(node):
        visited.add(node)
        stack.add(node)

        for neighbor in layer_graph[node]:
            if neighbor not in visited:
                if dfs(neighbor):
                    return True
            elif neighbor in stack:
                return True

        stack.remove(node)
        return False

    for node in layer_graph:
        if node not in visited:
            if dfs(node):
                return True
    return False


# ============================================================
# Walk Entire Project
# ============================================================

for root in [SRC_DIR, INCLUDE_DIR]:
    if not root.exists():
        continue
    for file in root.rglob("*"):
        if file.suffix in {".c", ".h"}:
            check_file(file)

# ============================================================
# Reporting
# ============================================================

print(f"{CYAN}--- Cleaner Architectural Enforcement ---{RESET}")
print(f"Files scanned: {files_scanned}")

if violations:
    print(f"{RED}Layer Violations Detected:{RESET}")
    for v in violations:
        print(f"  {RED}•{RESET} {v}")

if detect_cycles():
    print(f"{RED}Circular layer dependency detected!{RESET}")
    violations.append("layer cycle")

# ============================================================
# Exit Deterministically (CI Safe)
# ============================================================

if violations:
    print(f"{RED}FAILED — {len(violations)} architectural violation(s).{RESET}")
    sys.exit(1)
else:
    print(f"{GREEN}PASSED — No architectural violations.{RESET}")
    sys.exit(0)
