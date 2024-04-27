#!/bin/bash

CALLER_DIR=$(pwd)
cd "$(dirname "$0")"/..

python3 ./tool/amalgamation/amalgamate.py -c ./tool/amalgamation/fkYAML.json -s . --verbose=yes

cd $CALLER_DIR
