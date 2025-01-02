#!/bin/bash

SCRIPT_DIR="$(dirname "$0")"
ROOT_DIR="$SCRIPT_DIR/.."

# Platform switches are taken from https://gist.github.com/prabirshrestha/3080525
UNAME=$(uname)
if [ "$UNAME" = "Linux" ] || [ "$UNAME" = "Darwin" ]; then
    PYTHON_EXE=python3
    VENV_BINARY_DIR=bin
else
    # Use the Python launcher if the runtime environment is Windows.
    # No version option (`-<version>`) since running with the latest version should be enough.
    PYTHON_EXE=py
    # Binaries are stored in the `venv\Scripts` directory using Python venv module for Windows.
    VENV_BINARY_DIR=Scripts
fi


# install the clang-format package if not installed yet.
if [ ! -e "$SCRIPT_DIR"/venv_clang_format/"$VENV_BINARY_DIR"/clang-format ]; then
    "$PYTHON_EXE" -m venv "$SCRIPT_DIR/venv_clang_format"
    "$SCRIPT_DIR"/venv_clang_format/"$VENV_BINARY_DIR"/pip install clang-format==18.1.3
fi

for f in $(find "$ROOT_DIR/include" "$ROOT_DIR/tests" "$ROOT_DIR/examples" "$ROOT_DIR/tools/benchmark" -type f -name "*.hpp" -o -name "*.cpp" | sort); do
    "$SCRIPT_DIR"/venv_clang_format/"$VENV_BINARY_DIR"/clang-format "$f" -i
done
