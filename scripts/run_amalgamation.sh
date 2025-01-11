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
AMALGAMATION_TOOLS_DIR="$ROOT_DIR/tools/amalgamation"
"$PYTHON_EXE" "$AMALGAMATION_TOOLS_DIR"/amalgamate.py -c "$AMALGAMATION_TOOLS_DIR"/node_hpp.json -s . --verbose=yes
"$PYTHON_EXE" "$AMALGAMATION_TOOLS_DIR"/amalgamate.py -c "$AMALGAMATION_TOOLS_DIR"/fkyaml_fwd_hpp.json -s . --verbose=yes
