# Copilot instructions for fkYAML

## Project overview
- This repository contains fkYAML, a header-only C++ YAML library.
- The library is designed to be portable, dependency-light, and compatible with C++11 and later.
- Public API changes should preserve existing behavior unless the task explicitly requires otherwise.

## Coding guidelines
- Prefer small, localized changes that match the existing style of the codebase.
- Keep the implementation header-only friendly and avoid introducing new external dependencies.
- Follow the existing naming conventions:
  - namespaces: `fkyaml` and `detail`
  - macros: `FK_YAML_*`
  - functions and variables: snake_case
  - types: descriptive names such as `basic_node`, `node`, `node_type`
  - other conventions: described in the `.clang-format` and `.clang-tidy` files at the root of the repository.
- Preserve existing namespace and include patterns when editing headers under `include/fkYAML/`.
- Add the consistent copyright header to new files and maintain it in existing files.
- Keep comments and documentation concise and useful.

## Build and test
- The primary build system is CMake.
- Typical local validation flow:
  1. `cmake -S . -B build -DFK_YAML_BUILD_TEST=ON`
  2. `cmake --build build`
  3. `ctest --test-dir build --output-on-failure`
- If a change affects parsing, serialization, or node behavior, add or update tests in `tests/unit_test/`.

## Directory structure
- `/.github/`: GitHub-specific configuration files
  - `/.github/workflows/`: CI/CD workflow definitions
- `/cmake/`: CMake package configuration and helper files
- `/docs/`: Documentation files
  - `docs/`: Markdown documentation files
- `/examples/`: Example usage files (mainly referenced in the documentation)
- `/include/fkYAML/`: Core headers
  - `*.hpp`: Public headers
  - `detail/`: Implementation headers (not meant to be used directly by users)
    - `*.hpp`: Common headers
    - `conversions/*.hpp`: Conversion functions
    - `encodings/*.hpp`: Encoding/decoding functions
    - `input/*.hpp`: Deserialization functions from YAML to C++ types
    - `macros/*.hpp`: Macro definitions
    - `meta/*.hpp`: Type traits for metaprogramming
    - `output/*.hpp`: Serialization functions from C++ types to YAML
    - `types/*.hpp`: Enum type definitions
- `/LICENSES/`: license files for the repository dependencies
- `/scripts/`: Helper scripts for formatting, amalgamation, and repository maintenance
- `/single_include/fkYAML/`: Amalgamated single-header distribution
- `/tests/`: Test suite
  - `unit_test/`: Unit tests for all the functionalities of the library
  - `cmake_add_subdirectory_test/`: CMake integration test for add_subdirectory usage
  - `cmake_fetch_content_test/`: CMake integration test for FetchContent usage
  - `cmake_find_package_test/`: CMake integration test for find_package usage
  - `cmake_target_include_directories_test/`: CMake integration test for target_include_directories usage
- `/thirdparty/`: Vendored dependencies (for testing and CI workflows, not part of the public API)
- `/tools/`: Maintenance and benchmarking utilities

## Directory-specific guidance
- `include/fkYAML/`: treat public headers as the stable API surface; prefer backward-compatible changes.
- `include/fkYAML/detail/`: keep internal implementation headers consistent and avoid exposing unnecessary details.
- `single_include/fkYAML/`: keep the amalgamated single-header distribution aligned with changes in the main headers; regenerate it when necessary.
- `tests/`: keep the test suite organized and ensure changes are 100% covered by appropriate tests when there is any change in the codebase.
- `thirdparty/`: avoid modifying vendored dependencies unless the task explicitly requires it.
- `examples/`: keep examples simple, buildable, and consistent with the documented usage.
- `docs/`: update documentation when public behavior or APIs change.
- `tools/`: keep maintenance and benchmarking utilities compatible with the existing workflows.
- `scripts/`: preserve helper scripts for formatting, amalgamation, and repository maintenance.
- `cmake/`: keep package configuration and CMake helper files consistent with build changes.
- `build/`: generated build output; do not edit manually.

## Compatibility and safety
- Avoid breaking older compilers or C++11 compatibility.
- Do not introduce C++14/17-only constructs unless the task explicitly requires them and the repository already supports them.
- If a change affects the public API, consider updating related documentation and examples.
- Respect the repository license headers and existing REUSE metadata.
