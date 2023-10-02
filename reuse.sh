#!/bin/bash

pipx run reuse annotate include/fkYAML/*.hpp --template fkYAML \
    --copyright "Kensuke Fukutani <fktn.dev@gmail.com>" --copyright-style spdx \
    --license MIT  --year 2023 --style c
pipx run reuse annotate $(find test -type f -name '*.cpp' | sort) --template fkYAML_support \
    --copyright "Kensuke Fukutani <fktn.dev@gmail.com>" --copyright-style spdx \
    --license MIT  --year 2023 --style c
pipx run reuse annotate $(find tool -type f -name '*.cpp*' | sort) --template fkYAML_support \
    --copyright "Kensuke Fukutani <fktn.dev@gmail.com>" --copyright-style spdx \
    --license MIT  --year 2023 --style c

pipx run reuse --suppress-deprecation lint
