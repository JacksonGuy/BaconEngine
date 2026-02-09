#!/usr/bin/env bash
set -euo pipefail

if ! command -v clang-format >/dev/null 2>&1; then
    echo "Error: clang-format not found in PATH"
    exit 1
fi

echo "Formatting C++ files in src/ ..."

find src/ \
    -path "*/extern/*" -prune -o \
    -type f \( -name "*.cpp" -o -name "*.h" \) -print0 \
| xargs -0 clang-format -i --style=file

echo "Done."
