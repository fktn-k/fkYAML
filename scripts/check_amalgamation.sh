#!/bin/bash

set -eu

ROOT_DIR="$(dirname "$0")/.."

NODE_HPP_PATH="$ROOT_DIR/single_include/fkYAML/node.hpp"
FKYAML_FWD_HPP_PATH="$ROOT_DIR/single_include/fkYAML/fkyaml_fwd.hpp"

mv "$NODE_HPP_PATH" "$NODE_HPP_PATH~"
mv "$FKYAML_FWD_HPP_PATH" "$FKYAML_FWD_HPP_PATH~"

"$ROOT_DIR"/scripts/run_amalgamation.sh >/dev/null

if cmp -s "$NODE_HPP_PATH" "$NODE_HPP_PATH~" ; then
    echo Amalgamation check passed for node.hpp file!
else
    echo Amalgamation required for node.hpp file.
    echo Please follow the guideline in the CONTRIBUTING.md file.
fi
mv "$NODE_HPP_PATH~" "$NODE_HPP_PATH"

if cmp -s "$FKYAML_FWD_HPP_PATH" "$FKYAML_FWD_HPP_PATH~" ; then
    echo Amalgamation check passed for fkyaml_fwd.hpp file!
else
    echo Amalgamation required for fkyaml_fwd.hpp file.
    echo Please follow the guideline in the CONTRIBUTING.md file.
fi
mv "$FKYAML_FWD_HPP_PATH~" "$FKYAML_FWD_HPP_PATH"

