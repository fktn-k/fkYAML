#!/bin/bash

set -eu

ROOT_DIR="$(dirname "$0")/.."

SINGLE_HEADER_PATH="$ROOT_DIR/single_include/fkYAML/node.hpp"
mv "$SINGLE_HEADER_PATH" "$SINGLE_HEADER_PATH~"
"$ROOT_DIR"/scripts/run_amalgamation.sh >/dev/null

if cmp -s "$SINGLE_HEADER_PATH" "$SINGLE_HEADER_PATH~" ; then
    echo Amalgamation check passed!
else
    echo Amalgamation required.
    echo Please follow the guideline in the CONTRIBUTING.md file.
fi

mv "$SINGLE_HEADER_PATH~" "$SINGLE_HEADER_PATH"
