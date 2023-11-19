# **fkYAML version 0.2.1**

!!! abstract Release Packages

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.2.1/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.2.1/fkYAML.zip)

## Summary

This release supports the UTF-8 encoding (both escaped/unescaped) in parsing YAML documents.  
Furthermore, bugs in the deserialization feature have also been fixed.  

!!! warning CAVEAT

    The fkYAML library is an alpha quality software, at most.  
    It would take a while until this library gets stabled version, since implementation and maintenance has been/will be done by a single developer.  
    If you find a bug or wanted features, please open a discussion/issue to let me know.  
    Any kind of contribution is very much welcomed.  

## What's Changed

### :zap: Improvements

- \#209 support escaped unicode [\#215](https://github.com/fktn-k/fkYAML/pull/215) ([fktn-k](https://github.com/fktn-k))
- \#209 Support unescaped UTF-8 input characters in deserialization [\#210](https://github.com/fktn-k/fkYAML/pull/210) ([fktn-k](https://github.com/fktn-k))
- \#207 Support markers for the end of directives/documents [\#208](https://github.com/fktn-k/fkYAML/pull/208) ([fktn-k](https://github.com/fktn-k))
- \#190 Support specialization of deserialization for a vector of user-defined type objects [\#203](https://github.com/fktn-k/fkYAML/pull/203) ([fktn-k](https://github.com/fktn-k))
- \#195 Implement insertion/extraction operators for basic\_node template class [\#201](https://github.com/fktn-k/fkYAML/pull/201) ([fktn-k](https://github.com/fktn-k))


### :bug: Bug Fixes

- \#211 Fix indentation handling [\#213](https://github.com/fktn-k/fkYAML/pull/213) ([fktn-k](https://github.com/fktn-k))
- \#211 fixed bug in parsing single quoted strings [\#212](https://github.com/fktn-k/fkYAML/pull/212) ([fktn-k](https://github.com/fktn-k))
- \#205 Detect duplicate keys as an error [\#206](https://github.com/fktn-k/fkYAML/pull/206) ([fktn-k](https://github.com/fktn-k))
- \#200 Allow a space in unquoted strings [\#202](https://github.com/fktn-k/fkYAML/pull/202) ([fktn-k](https://github.com/fktn-k))

### :memo: Documentation

- Unified doxygen comment style [\#204](https://github.com/fktn-k/fkYAML/pull/204) ([fktn-k](https://github.com/fktn-k))

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.2.0...v0.2.1

---

# **fkYAML version 0.2.0**

!!! abstract "Release Packages"

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.2.0/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.2.0/fkYAML.zip)

## Summary

This release contains a breaking change in the default type for YAML mapping node: it will no longer preserve insertion order of internal key/value pairs by default.  
To preserve the insertion order as it used to, use `fkyaml::ordered_map` as `basic_node`'s mapping node type.  
Furthermore, factory methods for scalar values of `basic_node` type have been removed and unified into the `basic_node` constructors.  
Along with the above changes, internal operations in the fkYAML have also been improved regarding conversions between YAML nodes and values of native data types.  

!!! warning CAVEAT

    The fkYAML library is an alpha quality software, at most.  
    It would take a while until this library gets stabled version, since implementation and maintenance has been/will be done by a single developer.  
    If you find a bug or wanted features, please open a discussion/issue to let me know.  
    Any kind of contribution is very much welcomed.  

## What's Changed

### :boom: Breaking Changes

* \#180 clean up public apis [\#183](https://github.com/fktn-k/fkYAML/pull/183) ([fktn-k](https://github.com/fktn-k))
* \#167 Use std::map as the default mapping node type [\#174](https://github.com/fktn-k/fkYAML/pull/174) ([fktn-k](https://github.com/fktn-k))

### :zap: Improvements

* \#187 support compatible strings as an argument in basic\_node ctors [\#188](https://github.com/fktn-k/fkYAML/pull/188) ([fktn-k](https://github.com/fktn-k))
* \#180 Generalize getter APIs for node value references [\#184](https://github.com/fktn-k/fkYAML/pull/184) ([fktn-k](https://github.com/fktn-k))
* \#105 Support std::initializer\_list in basic\_node constructors [\#173](https://github.com/fktn-k/fkYAML/pull/173) ([fktn-k](https://github.com/fktn-k))

### :bug: Bug Fixes

* \#185 fixed missing calls for fclose\(\) in input adapter tests [\#186](https://github.com/fktn-k/fkYAML/pull/186) ([fktn-k](https://github.com/fktn-k))
* \#175 support detecting indentation in deserialization [\#176](https://github.com/fktn-k/fkYAML/pull/176) ([fktn-k](https://github.com/fktn-k))

### :robot: CI

* Updated header template and target file for version up script [\#197](https://github.com/fktn-k/fkYAML/pull/197) ([fktn-k](https://github.com/fktn-k))
* \#177 Add test for input adapters only [\#178](https://github.com/fktn-k/fkYAML/pull/178) ([fktn-k](https://github.com/fktn-k))

### :memo: Documentation

* \#179 Added integration with Google Analytics 4 [\#196](https://github.com/fktn-k/fkYAML/pull/196) ([fktn-k](https://github.com/fktn-k))
* \#179 Updated pages in the Home section in the documentation [\#194](https://github.com/fktn-k/fkYAML/pull/194) ([fktn-k](https://github.com/fktn-k))
* \#179 Migrate the gh-page content with MkDocs [\#193](https://github.com/fktn-k/fkYAML/pull/193) ([fktn-k](https://github.com/fktn-k))
* \#179 Created tutorial pages [\#192](https://github.com/fktn-k/fkYAML/pull/192) ([fktn-k](https://github.com/fktn-k))
* \#179 Migrate API docs with MkDocs [\#189](https://github.com/fktn-k/fkYAML/pull/189) ([fktn-k](https://github.com/fktn-k))
* \#179 introduce mkdocs for documentation [\#182](https://github.com/fktn-k/fkYAML/pull/182) ([fktn-k](https://github.com/fktn-k))

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.1.3...v0.2.0

---

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