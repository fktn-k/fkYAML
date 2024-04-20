#!/bin/bash

CALLER_DIR=$(pwd)
ROOT_DIR=$(cd "$(dirname "$0")" && cd ../.. && pwd)

# move to the directory where this script is placed.
cd test/clang_format

# install the clang-format package if not installed yet.
if [ ! -e ./venv/bin/clang-format ]; then
    python3 -m venv venv
    venv/bin/pip install clang-format==14.0.0
fi

for f in $(find "$ROOT_DIR/include" "$ROOT_DIR/test/unit_test" -type f -name "*.hpp" -o -name "*.cpp" | sort); do
    venv/bin/clang-format "$f" -i
done

cd "$CALLER_DIR"
