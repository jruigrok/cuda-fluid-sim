#!/usr/bin/env bash
set -euo pipefail

# Go to repo root (even if script is called from elsewhere)
REPO_ROOT=$(git rev-parse --show-toplevel)
cd "$REPO_ROOT"

# Check clang-format
if ! command -v clang-format >/dev/null 2>&1; then
  echo "clang-format not found. Please install it."
  exit 1
fi

echo "Formatting source files..."

# File extensions to format
EXTENSIONS='\.(c|cc|cpp|cxx|h|hh|hpp|hxx)$'

# Format only git-tracked files
FILES=$(git ls-files | grep -E "$EXTENSIONS" || true)

if [ -z "$FILES" ]; then
  echo "No files to format."
  exit 0
fi

# Format in-place
echo "$FILES" | xargs clang-format -i

echo "Formatting complete"

