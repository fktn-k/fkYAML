# **fkYAML version 0.1.3**

!!! abstract "Release Packages"

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.1.3/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.1.3/fkYAML.zip)

## Summary
This release contains improvements of the fkYAML library's internal operations with some reorganization of CMake scripts.  
In addition, the analysis result from Codacy has become shown at the top of the README.md file.  

!!! warning "CAVEAT"

    The fkYAML library is an alpha quality software, at most.  
    It would take a while until this library gets stabled version, since implementation and maintenance has been/will be done by a single developer.  
    If you find a bug or wanted features, please open a discussion/issue to let me know.  
    Any kind of contribution is very much welcomed.  

## What's Changed

### :zap: Improvements
* \#145 Expand swap() support for basic node by @fktn-k in https://github.com/fktn-k/fkYAML/pull/163
* \#18 Resolve warnings while building library/tests by @fktn-k in https://github.com/fktn-k/fkYAML/pull/165
* \#164 Use default initial values for class member variables by @fktn-k in https://github.com/fktn-k/fkYAML/pull/168
* \#158 Added to_string() for serialization of YAML nodes. by @fktn-k in https://github.com/fktn-k/fkYAML/pull/170
* \#166 Organize CMake scripts for testing by @fktn-k in https://github.com/fktn-k/fkYAML/pull/171

### :hammer: Tool/Service Integrations
* \#160 Added the Codacy badge to README.md by @fktn-k in https://github.com/fktn-k/fkYAML/pull/162

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.1.2...v0.1.3

---

# **fkYAML version 0.1.2**

## Summary
This release contains improvements regarding internal implementation of the fkYAML library.  
Furthermore, new unit test cases have been added and the coverage result (C1) is now 100%.  

!!! warning "CAVEAT"

    The fkYAML library is an alpha quality software, at most.  
    It would take a while until this library gets stabled version, since implementation and maintenance has been/will be done by a single developer.  
    If you find a bug or wanted features, please open a discussion/issue to let me know.  
    Any kind of contribution is very much welcomed.  

## What's Changed

### :zap: Improvements
* \#133 refactor lexer by @fktn-k in https://github.com/fktn-k/fkYAML/pull/146

### :bug: Bug Fixes
* Fix version_macros.hpp file path in Makefile by @fktn-k in https://github.com/fktn-k/fkYAML/pull/147

### :robot: CI
* \#150 Add unit tests for input_handler class by @fktn-k in https://github.com/fktn-k/fkYAML/pull/151
* \#150 Excluded unreachable lines in serializer class test from coverage data by @fktn-k in https://github.com/fktn-k/fkYAML/pull/152
* \#150 Covered a missing branch in ordered_map class by @fktn-k in https://github.com/fktn-k/fkYAML/pull/153
* \#150 added/modified unit tests for lexical_analyzer test by @fktn-k in https://github.com/fktn-k/fkYAML/pull/154
* \#150 Added tests  for basic_deserializer class by @fktn-k in https://github.com/fktn-k/fkYAML/pull/155

### :memo: Documentation
* Updated documents by @fktn-k in https://github.com/fktn-k/fkYAML/pull/156

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.1.1...v0.1.2

---

# **fkYAML version 0.1.1**

## Summary
This release contains a hotfix for the test suite to check CMake integration with FetchContent module which uses the old fkYAML APIs.  
All changes are backward-compatible. See [release notes of version 0.1.0](https://github.com/fktn-k/fkYAML/releases/tag/v0.1.0) for more information on other features.  

### CAVEAT
The fkYAML library is a alpha quality software.
Implementation and maintenance has been/will be done by a single developer.
If you find a bug or wanted features, please open a discussion/issue to let us know. Any kind of contribution is very much welcomed.

## What's Changed
### :bug: Bug Fixes
- use the v0.1.0 fkYAML APIs in the cmake_fetch_content_test (commit hash: https://github.com/fktn-k/fkYAML/commit/2253217c4fe4d83cfe845d77675c3a1ae412c62e) by @fktn-k 

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.1.0...v0.1.1

---

# **fkYAML version 0.1.0**

## Summary
This release contains a huge change in public APIs, including naming conventions and migration of serialization/deserialization features to fkyaml::basic_node class APIs.  
Furthermore, internal implementation of the fkYAML library has been intentionally moved to `include/fkYAML/detail`'s subdirectories so that we can provide as much backward compatibility as possible for the future releases.  

### CAVEAT  
The fkYAML library is a alpha quality software.  
Implementation and maintenance has been/will be done by a single developer.  
If you find a bug or wanted features, please open a discussion/issue to let us know. Any kind of contribution is very much welcomed.  

## What's Changed
### :zap: Improvements
* \#123 Change naming conventions to lower_snake_case by @fktn-k in https://github.com/fktn-k/fkYAML/pull/124
* \#123 Changed file naming convention by @fktn-k in https://github.com/fktn-k/fkYAML/pull/125
* \#127 Generalize conversions between nodes and native data by @fktn-k in https://github.com/fktn-k/fkYAML/pull/129
* \#128 moved internal impl to detail dir/namespace by @fktn-k in https://github.com/fktn-k/fkYAML/pull/131
* \#126 Generalize serialization/deserialization features by @fktn-k in https://github.com/fktn-k/fkYAML/pull/134
* \#132 Classify source files in detail directory by @fktn-k in https://github.com/fktn-k/fkYAML/pull/137
* \#135 optimize build scripts by @fktn-k in https://github.com/fktn-k/fkYAML/pull/138

### :memo: Documentation
* \#127 Update examples in readme by @fktn-k in https://github.com/fktn-k/fkYAML/pull/141

### :robot: CI
* \#136 Auto-generate a helper source file for the clang-tidy tool by @fktn-k in https://github.com/fktn-k/fkYAML/pull/139
* \#128 fixed filtering source files to generate full-spec coverage by @fktn-k in https://github.com/fktn-k/fkYAML/pull/140

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.0.0...v0.1.0

---

# **fkYAML version 0.0.1**

## Summary
This release improves internal operations in the fkYAML library and adds unit tests for the existing classes.  
Furthermore, several tool/service integrations has been incorporated for better maintainability.  

## What's Changed

### :zap: Improvements
* \#34 add unit tests for deserializer by @fktn-k in https://github.com/fktn-k/fkYAML/pull/97
* Expand usable key types by @fktn-k in https://github.com/fktn-k/fkYAML/pull/115

### :hammer: Tool/Service Integrations
* \#101 added memory leak check with Valgrind by @fktn-k in https://github.com/fktn-k/fkYAML/pull/108
* \#116 use reuse software for file headers by @fktn-k in https://github.com/fktn-k/fkYAML/pull/117
* \#118 introduce cmake-format by @fktn-k in https://github.com/fktn-k/fkYAML/pull/119
* \#120 added target to use github-changelog-generator tool by @fktn-k in https://github.com/fktn-k/fkYAML/pull/121

### :robot: CI
* \#109 Use official docker image for GCC during CI jobs by @fktn-k in https://github.com/fktn-k/fkYAML/pull/110
* \#111 Use docker images for Clang compilers during CI jobs by @fktn-k in https://github.com/fktn-k/fkYAML/pull/112

### :memo: Documentation
* \#113 Generate API documentation only for public members by @fktn-k in https://github.com/fktn-k/fkYAML/pull/114

### 🧑‍🤝‍🧑 Community
* \#21 Update issue templates by @fktn-k in https://github.com/fktn-k/fkYAML/pull/100


**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.0.0...v0.0.1

---

# **fkYAML v0.0.0**

This release provides basic features of the fkYAML library.  
Major known issues and limitaions are as follows:  

- %TAG directives are not supported. (Will surely be implemented.)
- YAML key must be a string literal. (YAML specification of any version does not restrict YAML keys as such.)
- Backward compatibilities for YAML 1.1 or older have not yet been implemented.

The other issues and limitations will also be fixed in later versions.  
Wait for such a version, or create a PR to fix one.  