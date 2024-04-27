#!/bin/bash

SCRIPT_DIR="$(dirname "$0")"
ROOT_DIR="$SCRIPT_DIR/.."

# install the clang-format package if not installed yet.
if [ ! -e "$SCRIPT_DIR"/venv_clang_format/bin/clang-format ]; then
    python3 -m venv "$SCRIPT_DIR/venv_clang_format"
    "$SCRIPT_DIR"/venv_clang_format/bin/pip install clang-format==14.0.0
fi

for f in $(find "$ROOT_DIR/include" "$ROOT_DIR/test" "$ROOT_DIR/docs/examples" -type f -name "*.hpp" -o -name "*.cpp" | sort); do
    "$SCRIPT_DIR"/venv_clang_format/bin/clang-format "$f" -i
done
