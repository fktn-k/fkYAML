.PHONY: update-version-macros CHANGELOG.md update-version examples fkYAML.natvis

#################
#   variables   #
#################

# list of source files in the include directory.
SRCS = $(shell find include -type f -name '*.hpp' | sort)
# list of sources in the test directory.
TEST_SRCS = $(shell find test -type f -name '*.hpp' -o -name '*.cpp' | sort)
# list of sources in the examples directory.
EXAMPLE_SRCS = $(shell find docs/examples -type f -name '*.cpp' | sort)
# list of sources in the tool directory.
TOOL_SRCS = $(shell find tool -type f -name '*.cpp' | sort)

# target version definition
TARGET_MAJOR_VERSION := 0
TARGET_MINOR_VERSION := 3
TARGET_PATCH_VERSION := 9
TARGET_VERSION_FULL := $(TARGET_MAJOR_VERSION).$(TARGET_MINOR_VERSION).$(TARGET_PATCH_VERSION)
VERSION_MACRO_FILE := include/fkYAML/detail/macros/version_macros.hpp

# system
JOBS = $(($(shell grep cpu.cores /proc/cpuinfo | sort -u | sed 's/[^0-9]//g') + 1))

###############################################
#   documentation of the Makefile's targets   #
###############################################

all:
	@echo "amalgamate - amalgamate sources of the fkYAML library and generate its single header version."
	@echo "clang-format - check whether source files are well formatted."
	@echo "clang-sanitizers - check whether no runtime issue is detected while running the unit test app."
	@echo "clang-tidy - check whether source files detect no issues during static code analysis."
	@echo "fkYAML.natvis - generate the Natvis debugger visualization file."
	@echo "html-coverage - generate HTML coverage report."
	@echo "iwyu - check whether source files are each self-contained."
	@echo "lcov-coverage - generate coverage data with lcov."
	@echo "reuse - update file header descriptions in source files."
	@echo "update-version - update version descriptions in the project."
	@echo "valgrind - check whether no memory leak is detected while running the unit test app."

#############################
#   Static Code Analyzers   #
#############################

clang-format:
	./scripts/run_clang_format.sh

# pre-requisites: clang-tidy
clang-tidy:
	cmake -B build_clang_tidy -S . -DFK_YAML_RUN_CLANG_TIDY=ON
	cmake --build build_clang_tidy --target run_clang_tidy -j $(JOBS)

# pre-requisites: iwyu
iwyu:
	cmake -B build_iwyu -S . -DFK_YAML_RUN_IWYU=ON
	cmake --build build_iwyu --target run_iwyu -j $(JOBS)

##############################
#   Runtime Code Analyzers   #
##############################

# pre-requisites: clang
clang-sanitizers:
	CXX=clang++ cmake -B build_clang_sanitizers -S . -DCMAKE_BUILD_TYPE=Debug -DFK_YAML_BUILD_TEST=ON -DFK_YAML_RUN_CLANG_SANITIZERS=ON
	cmake --build build_clang_sanitizers --config Debug -j $(JOBS)
	ctest -C Debug --test-dir build_clang_sanitizers --output-on-failure -j $(JOBS)

# pre-requisites: valgrind
valgrind:
	cmake -B build_valgrind -S . -DCMAKE_BUILD_TYPE=Debug -DFK_YAML_BUILD_TEST=ON -DFK_YAML_RUN_VALGRIND=ON
	cmake --build build_valgrind --config Debug -j $(JOBS)
	ctest -C Debug -T memcheck --test-dir build_valgrind --output-on-failure -j $(JOBS)

###########################
#   Source Amalgamation   #
###########################

amalgamate:
	./scripts/run_amalgamation.sh

check-amalgamate:
	./scripts/check_amalgamation.sh

##########################################
#   Natvis Debugger Visualization File   #
##########################################

update-params-for-natvis:
	echo { \"version\": \"$(TARGET_VERSION_FULL)\" } > ./tool/natvis_generator/params.json

fkYAML.natvis: update-params-for-natvis
	@$(MAKE) -C ./tool/natvis_generator generate

#####################
#   Documentation   #
#####################

examples:
	cmake -B build_examples -S . -DCMAKE_BUILD_TYPE=Debug -DFK_YAML_BUILD_EXAMPLES=ON
	cmake --build build_examples --config Debug

build-docs: examples
	@$(MAKE) -C ./docs/mkdocs build

serve-docs: examples
	@$(MAKE) -C ./docs/mkdocs serve

###############
#   Version   #
###############

# update version description in reuse template files
update-reuse-templates:
	$(shell find .reuse/templates -type f -name '*.jinja2' -exec sed -i 's/version [0-9]\+\.[0-9]\+\.[0-9]\+/version $(TARGET_VERSION_FULL)/' {} \;)

update-version-macros:
	$(shell sed -i 's/FK_YAML_MAJOR_VERSION != [0-9]\+/FK_YAML_MAJOR_VERSION != $(TARGET_MAJOR_VERSION)/' $(VERSION_MACRO_FILE))
	$(shell sed -i 's/FK_YAML_MINOR_VERSION != [0-9]\+/FK_YAML_MINOR_VERSION != $(TARGET_MINOR_VERSION)/' $(VERSION_MACRO_FILE))
	$(shell sed -i 's/FK_YAML_PATCH_VERSION != [0-9]\+/FK_YAML_PATCH_VERSION != $(TARGET_PATCH_VERSION)/' $(VERSION_MACRO_FILE))
	$(shell sed -i 's/#define FK_YAML_MAJOR_VERSION [0-9]\+/#define FK_YAML_MAJOR_VERSION $(TARGET_MAJOR_VERSION)/' $(VERSION_MACRO_FILE))
	$(shell sed -i 's/#define FK_YAML_MINOR_VERSION [0-9]\+/#define FK_YAML_MINOR_VERSION $(TARGET_MINOR_VERSION)/' $(VERSION_MACRO_FILE))
	$(shell sed -i 's/#define FK_YAML_PATCH_VERSION [0-9]\+/#define FK_YAML_PATCH_VERSION $(TARGET_PATCH_VERSION)/' $(VERSION_MACRO_FILE))

update-project-version:
	$(shell sed -i 's/VERSION [0-9]\+\.[0-9]\+\.[0-9]\+/VERSION $(TARGET_VERSION_FULL)/' CMakeLists.txt)

# pre-requisites: pipx, reuse
reuse: update-reuse-templates
	pipx run reuse annotate $(SRCS) --template fkYAML \
		--copyright "Kensuke Fukutani <fktn.dev@gmail.com>" --copyright-style spdx \
		--license MIT --year "2023-2024" --style c
	pipx run reuse annotate $(TEST_SRCS) $(EXAMPLE_SRCS) $(TOOL_SRCS) --template fkYAML_support \
		--copyright "Kensuke Fukutani <fktn.dev@gmail.com>" --copyright-style spdx \
		--license MIT --year "2023-2024" --style c
	pipx run reuse lint

update-sources: reuse update-version-macros
	@$(MAKE) amalgamate

update-git-tag-ref:
	$(shell sed -i 's/GIT_TAG \+v[0-9]\+\.[0-9]\+\.[0-9]\+/GIT_TAG v$(TARGET_VERSION_FULL)/' docs/mkdocs/docs/tutorials/cmake_integration.md)
	$(shell sed -i 's/GIT_TAG \+v[0-9]\+\.[0-9]\+\.[0-9]\+/GIT_TAG v$(TARGET_VERSION_FULL)/' test/cmake_fetch_content_test/project/CMakeLists.txt)

CHANGELOG.md:
	github_changelog_generator --user fktn-k --project fkYAML \
		--no-issues \
		--simple-list \
		--exclude-labels release,ignored \
		--release-url https://github.com/fktn-k/fkYAML/releases/tag/%s \
		--future-release v$(TARGET_VERSION_FULL)

update-version: fkYAML.natvis update-project-version update-sources update-git-tag-ref CHANGELOG.md
	@echo "updated version to $(TARGET_VERSION_FULL)"

################
#   Coverage   #
################

# pre-requisites: lcov
lcov-coverage:
	cmake -B build_coverage -S . -DCMAKE_BUILD_TYPE=Debug -DFK_YAML_CODE_COVERAGE=ON
	cmake --build build_coverage --config Debug --target generate_test_coverage -j $(JOBS)

# pre-requisites: genhtml lcov
html-coverage: lcov-coverage
	genhtml build_coverage/coverage/fkYAML.info --output-directory build_coverage/html \
		--title "fkYAML: A C++ header-only YAML library" \
		--legend --demangle-cpp --show-details --branch-coverage

#################
#   Benchmark   #
#################

bm-debug:
	cmake -B build_bm_debug -S . -DCMAKE_BUILD_TYPE=Debug -DFK_YAML_RUN_BENCHMARK=ON
	cmake --build build_bm_debug --config Debug
	./build_bm_debug/tool/benchmark/benchmarker ./tool/benchmark/macos.yml > ./tool/benchmark/result_debug.log

bm-release:
	cmake -B build_bm_release -S . -DCMAKE_BUILD_TYPE=Release -DFK_YAML_RUN_BENCHMARK=ON
	cmake --build build_bm_release --config Release
	./build_bm_release/tool/benchmark/benchmarker ./tool/benchmark/macos.yml > ./tool/benchmark/result_release.log

###################
#   Maintenance   #
###################

clean:
	rm -rf \
		build \
		build_bm_debug \
		build_bm_release \
		build_clang_format \
		build_clang_sanitizers \
		build_clang_tidy \
		build_coverage \
		build_examples \
		build_iwyu \
		build_valgrind
