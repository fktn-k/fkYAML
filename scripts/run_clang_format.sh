#!/bin/bash

CALLER_DIR=$(pwd)
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
ROOT_DIR=$(cd ../.. && pwd)

cd $SCRIPT_DIR

# install the clang-format package if not installed yet.
if [ ! -e $SCRIPT_DIR/venv/bin/clang-format ]; then
    python3 -m venv venv_clang_format
    venv/bin/pip install clang-format==14.0.0
fi

for f in $(find "$ROOT_DIR/include" "$ROOT_DIR/test" "$ROOT_DIR/docs/examples" -type f -name "*.hpp" -o -name "*.cpp" | sort); do
    $SCRIPT_DIR/venv_clang_format/bin/clang-format "$f" -i
done

cd $CALLER_DIR
