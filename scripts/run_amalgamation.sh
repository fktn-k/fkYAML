#!/bin/bash

CALLER_DIR=$(pwd)
ROOT_DIR=$(cd "$(dirname "$0")" && cd ../.. && pwd)

python3 ./tool/amalgamation/amalgamate.py -c ./tool/amalgamation/fkYAML.json -s . --verbose=yes

cd $CALLER_DIR
