#!/bin/bash

ROOT_DIR="$(dirname "$0")"/..
python3 "$ROOT_DIR"/tool/amalgamation/amalgamate.py -c "$ROOT_DIR"/tool/amalgamation/fkYAML.json -s . --verbose=yes
