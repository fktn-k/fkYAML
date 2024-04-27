#!/bin/bash

set -e

CALLER_DIR=$(pwd)
ROOT_DIR=$(cd "$(dirname "$0")" && cd ../.. && pwd)

SINGLE_HEADER_PATH=$ROOT_DIR/single_include/fkYAML/node.hpp
mv $SINGLE_HEADER_PATH $SINGLE_HEADER_PATH~
./scripts/run_amalgamation.sh
diff $SINGLE_HEADER_PATH $SINGLE_HEADER_PATH~ || \
    (echo Amalgamation required. ; \
     echo Please follow the guideline in the CONTRIBUTING.md file. ; \
     mv $SINGLE_HEADER_PATH~ $SINGLE_HEADER_PATH ; \
     cd $CALLER_DIR ; false)
mv $SINGLE_HEADER_PATH~ $SINGLE_HEADER_PATH

echo Amalgamation check passed!

cd $CALLER_DIR
