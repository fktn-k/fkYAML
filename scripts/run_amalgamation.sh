#!/bin/bash

set -eu

# Platform switches are taken from https://gist.github.com/prabirshrestha/3080525
UNAME=$(uname)
if [ "$UNAME" = "Linux" ] || [ "$UNAME" = "Darwin" ]; then
    PYTHON_EXE=python3
else
    # Use the Python launcher if the runtime environment is Windows.
    # No version option (`-<version>`) since running with the latest version should be enough.
    PYTHON_EXE=py
fi

ROOT_DIR="$(dirname "$0")"/..
AMALGAMATION_TOOL_DIR="$ROOT_DIR/tool/amalgamation"
"$PYTHON_EXE" "$AMALGAMATION_TOOL_DIR"/amalgamate.py -c "$AMALGAMATION_TOOL_DIR"/fkYAML.json -s . --verbose=yes
