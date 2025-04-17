# Releases

## **fkYAML version 0.4.2**

!!! abstract "Release Packages"

    * CMake package of the multiple header version
        * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.4.2/fkYAML.zip)
        * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.4.2/fkYAML.tgz)
    * CMake package of the single header version
        * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.4.2/fkYAML_single_header.zip)
        * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.4.2/fkYAML_single_header.tgz)
    * minimum repository contents for CMake
        * [fkYAML_min.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.4.2/fkYAML_min.zip)
        * [fkYAML_min.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.4.2/fkYAML_min.tgz)
    * single headers
        * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.4.2/node.hpp)
        * [fkyaml_fwd.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.4.2/fkyaml_fwd.hpp)

### What's Changed

#### :sparkles: New Features
- Add to\_node\(\) implementations for STL container types ([\#465](https://github.com/fktn-k/fkYAML/pull/465), [fktn-k](https://github.com/fktn-k))
    - `fkyaml::basic_node` objects can now be constructible with objects with most of the STL container types.
      ```cpp
      std::list<int> l {123, -456, 789};
      fkyaml::node seq = l; // internally converted as a sequence with the built-in to_node implementation.

      std::unordered_map<std::string, float> um {{"foo", 3.14f}, {"bar", 1.41f}};
      fkyaml::node map = um; // internally converted as a mapping with the built-in to_node implementation.
      ```
    - See [the API reference page](https://fktn-k.github.io/fkYAML/api/basic_node/constructor/) for the `fkyaml::basic_node` constructor for more details.

#### :zap: Improvements
- Improve handling node properties in deserialization ([\#463](https://github.com/fktn-k/fkYAML/pull/463), [fktn-k](https://github.com/fktn-k))
- Allow ':' adjacent to a flow mapping value right after a JSON-like key ([\#468](https://github.com/fktn-k/fkYAML/pull/468), [fktn-k](https://github.com/fktn-k))
- Refactor UTF encode validation & line break normalization ([\#470](https://github.com/fktn-k/fkYAML/pull/470), [fktn-k](https://github.com/fktn-k))

#### :bug: Bug Fixes
- Fix conversions from empty tuples ([\#467](https://github.com/fktn-k/fkYAML/pull/467), [fktn-k](https://github.com/fktn-k))
- Fix comment prefix check ([\#469](https://github.com/fktn-k/fkYAML/pull/469), [fktn-k](https://github.com/fktn-k))
- Fix error in parsing empty inputs ([\#471](https://github.com/fktn-k/fkYAML/pull/471), [fktn-k](https://github.com/fktn-k))
- Fix document marker detections ([\#472](https://github.com/fktn-k/fkYAML/pull/472), [fktn-k](https://github.com/fktn-k))

#### :robot: CI
- Disable the fail-fast option for GitHub Actions workflow jobs ([\#466](https://github.com/fktn-k/fkYAML/pull/466), [fktn-k](https://github.com/fktn-k))

#### :memo: Documentation
- Fix example builds and typos in docs ([\#464](https://github.com/fktn-k/fkYAML/pull/464), [fktn-k](https://github.com/fktn-k))
- Fix example builds with incorrectly indented document markers ([\#473](https://github.com/fktn-k/fkYAML/pull/473), [fktn-k](https://github.com/fktn-k))

### Full Changelog
https://github.com/fktn-k/fkYAML/compare/v0.4.1...v0.4.2

---

## **fkYAML version 0.4.1**

!!! abstract "Release Packages"

    * CMake package of the multiple header version
        * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.4.1/fkYAML.zip)
        * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.4.1/fkYAML.tgz)
    * CMake package of the single header version
        * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.4.1/fkYAML_single_header.zip)
        * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.4.1/fkYAML_single_header.tgz)
    * minimum repository contents for CMake
        * [fkYAML_min.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.4.1/fkYAML_min.zip)
        * [fkYAML_min.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.4.1/fkYAML_min.tgz)
    * single headers
        * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.4.1/node.hpp)
        * [fkyaml_fwd.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.4.1/fkyaml_fwd.hpp)

### What's Changed

#### :sparkles: New Features

- Add get\_value\_inplace API to basic\_node ([\#443](https://github.com/fktn-k/fkYAML/pull/443), [fktn-k](https://github.com/fktn-k))
    - The new API `get_value_inplace()` converts a node value into a given type and writes the result into a given output parameter.
    - The target type doesn't have to be default-constructible since the API doesn't construct an instance during the conversion.
    - See [the dedicated API reference page](https://fktn-k.github.io/fkYAML/api/basic_node/get_value_inplace/) for more details.
- Mapping key/value access in range based for loops ([\#458](https://github.com/fktn-k/fkYAML/pull/458), [fktn-k](https://github.com/fktn-k))
    - Proposed by [sndth](https://github.com/sndth) in the discussion [\#400](https://github.com/fktn-k/fkYAML/discussions/400)
    - The new API `map_items()` allows accessing mapping keys/values in range-based for loops.
    - Structured binding support is added to `fkyaml::basic_node::iterator` and `fkyaml::basic_node::const_iterator`.
      ```cpp
      fkyaml::node mapping = {{"a", 123}, {"b", 3.14}};
      for (auto& [key, val] : mapping.map_items()) {
          // `key` and `val` are both of type `fkyaml::node::reference`.
          std::cout << "key: " << key << ", value: " << val << std::endl;
      }
      // output:
      // key: a, value: 123
      // key: b, value: 3.14
      ```
    - See [the dedicated API reference page](https://fktn-k.github.io/fkYAML/api/basic_node/map_items/) for more details.
- support std::forward\_list in from\_node ([\#444](https://github.com/fktn-k/fkYAML/pull/444), [fktn-k](https://github.com/fktn-k))

#### :zap: Improvements

- Resolve CMake deprecation warning ([\#442](https://github.com/fktn-k/fkYAML/pull/442), [fktn-k](https://github.com/fktn-k))
- Optimize lexer implementation ([\#448](https://github.com/fktn-k/fkYAML/pull/448), [fktn-k](https://github.com/fktn-k))

#### :bug: Bug Fixes

- Emit an error if ":" is missing after a mapping key ([\#450](https://github.com/fktn-k/fkYAML/pull/450), [fktn-k](https://github.com/fktn-k))
    - Reported by [sndth](https://github.com/sndth) in the issue [\#449](https://github.com/fktn-k/fkYAML/issues/449)
- Fix bad indentation detection for block collection entries ([\#451](https://github.com/fktn-k/fkYAML/pull/451), [fktn-k](https://github.com/fktn-k))
    - Reported by [sndth](https://github.com/sndth) in the issue [\#449](https://github.com/fktn-k/fkYAML/issues/449)
- Fix serialization of empty collections
    - fix: Fixed a bug where serialize didn't write out empty vectors and mappings ([\#456](https://github.com/fktn-k/fkYAML/pull/456), [johan-gson](https://github.com/johan-gson))
    - Add test cases for serializing empty collection nodes ([\#457](https://github.com/fktn-k/fkYAML/pull/457), [fktn-k](https://github.com/fktn-k))
    - Fix: made sure that empty documents just serialize to \[\] or {} ([\#460](https://github.com/fktn-k/fkYAML/pull/460), [johan-gson](https://github.com/johan-gson))
    - Add test cases for serializing root empty collections ([\#461](https://github.com/fktn-k/fkYAML/pull/461), [fktn-k](https://github.com/fktn-k))

#### :robot: CI

- Update workflow jobs for release artifacts ([\#447](https://github.com/fktn-k/fkYAML/pull/447), [fktn-k](https://github.com/fktn-k))
    - Adds new release artifacts (fkYAML_min.zip, fkYAML_min.tgz) which contain minimum required files for CMake. (roughly the top CMakeLists.txt & include|single_include directories)
    - They reduces the cost of downloading unnecessary files for a client app by using the CMake FetchContent module.  
      ```cmake
      FetchContent_Declare(
          fkYAML
          URL https://github.com/fktn-k/fkYAML/releases/download/v0.4.1/fkYAML_min.zip
      )
      FetchContent_MakeAvailable(fkYAML)
      ```

#### :memo: Documentation

- Revise the contents of API References ([\#445](https://github.com/fktn-k/fkYAML/pull/445), [fktn-k](https://github.com/fktn-k))
- Update copyright year and directory structure ([\#452](https://github.com/fktn-k/fkYAML/pull/452), [fktn-k](https://github.com/fktn-k))
- Fix invalid paths for documentation & other tools ([\#453](https://github.com/fktn-k/fkYAML/pull/453), [fktn-k](https://github.com/fktn-k))

### Full Changelog
https://github.com/fktn-k/fkYAML/compare/v0.4.0...v0.4.1

---

## **fkYAML version 0.4.0**

!!! abstract "Release Packages"

    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.4.0/fkYAML.zip)
    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.4.0/fkYAML.tgz)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.4.0/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.4.0/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.4.0/node.hpp) (single header)
    * [fkyaml_fwd.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.4.0/fkyaml_fwd.hpp) (single header)

### Summary
This release adds new features: parsing multi-line plain scalars & reverse iterations over sequence/mapping node elements.  
See the related pull requests and documentations for more details.  

A number of bugs (mostly in the deserialization feature) have also been resolved.  
Note that a breaking change has been made in the way of error handling when deserializing an int or float scalar fail. The library now emits no error on such a scalar and treats it as a string scalar instead.

### What's Changed

#### :boom: Breaking Changes
- Stop throwing parse\_error on string-to-int/float conversion failures if not forced with tag ([\#431](https://github.com/fktn-k/fkYAML/pull/431), [fktn-k](https://github.com/fktn-k))
    - reported by [tomwpid](https://github.com/tomwpid) in the issue [\#428](https://github.com/fktn-k/fkYAML/issues/428)
    - The library used to throw a `fkyaml::parse_error` upon conversion failures from a scalar to an integer or floating point value while parsing a YAML like this:  
      ```yaml
      id: 6E-578 # "6E-578" is interpreted as a floating point value but not expressible as a `double`
                # --> `fkyaml::parse_error` gets thrown due to the conversion failure.
      ```
    - Such a conversion failure is now internally recovered by treating the scalar as a string scalar instead.  

#### :sparkles: New Features
- Support parsing multiline plain scalars ([\#432](https://github.com/fktn-k/fkYAML/pull/432), [fktn-k](https://github.com/fktn-k))
    - Parsing a YAML which contains multi-line plain (unquoted) scalars are now supported.  
      ```yaml
      foo: this is 
        a multi-line
          plain scalar # interpreted as "this is a multi-line plain scalar"
      ```
- Support reverse iterations over sequence/mapping nodes ([\#440](https://github.com/fktn-k/fkYAML/pull/440), [fktn-k](https://github.com/fktn-k))
    - You can now iterate over sequence/mapping elements in a reversed order like this:  
      ```cpp
      // node is [1, 2, 3]
      for (auto rit = node.rbegin(); rit != node.rend(); ++rit) {
          std::cout << *rit << std::endl;
      }
      
      // output:
      // 3
      // 2
      // 1
      ```

#### :zap: Improvements
- Resolve the C4800 warning when compiled with MSVC ([\#430](https://github.com/fktn-k/fkYAML/pull/430), [fktn-k](https://github.com/fktn-k))
    - reported by [tomwpid](https://github.com/tomwpid) in the issue [\#429](https://github.com/fktn-k/fkYAML/issues/429)
- Make node iterators compatible with different value type const-ness ([\#438](https://github.com/fktn-k/fkYAML/pull/438), [fktn-k](https://github.com/fktn-k))
    - `fkyaml::node::iterator` and `fkyaml::node::const_iterator` are compatible in constructions, assignments and comparisons.  

#### :bug: Bug Fixes
- Emit error if an anchor is specified to an alias ([\#434](https://github.com/fktn-k/fkYAML/pull/434), [fktn-k](https://github.com/fktn-k))
- Fixed bugs in parsing block scalars ([\#435](https://github.com/fktn-k/fkYAML/pull/435), [fktn-k](https://github.com/fktn-k))
- Fix parsing input which begins with a newline & indentation ([\#437](https://github.com/fktn-k/fkYAML/pull/437), [fktn-k](https://github.com/fktn-k))
- Fix round-trip issue in float serialization using scientific notation ([\#439](https://github.com/fktn-k/fkYAML/pull/439), [fktn-k](https://github.com/fktn-k))
    - reported by [dyerbod](https://github.com/dyerbod) in the issue [\#405](https://github.com/fktn-k/fkYAML/issues/405)

#### :robot: CI
- Update GitHub Actions workflow jobs using macOS related runner images ([\#433](https://github.com/fktn-k/fkYAML/pull/433), [fktn-k](https://github.com/fktn-k))
- Add more GCC & Clang versions to use in GitHub Actions workflows ([\#436](https://github.com/fktn-k/fkYAML/pull/436), [fktn-k](https://github.com/fktn-k))

### Full Changelog
https://github.com/fktn-k/fkYAML/compare/v0.3.14...v0.4.0

## **fkYAML version 0.3.14**

!!! abstract "Release Packages"

    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.14/fkYAML.zip)
    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.14/fkYAML.tgz)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.14/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.14/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.14/node.hpp) (single header)
    * [fkyaml_fwd.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.14/fkyaml_fwd.hpp) (single header)

### Summary
This release adds the new header file *fkyaml_fwd.hpp* which provides the library's namespace macros and forward declarations of the library's API classes. With the file, you can reduce the compile time cost of including the full library header if some source files do not actually use the library features.  
Furthermore, conversions of YAML nodes into arbitrary types have been much improved and a lot more STL container and scalar types have been supported for that purpose. See [the API documentation page](https://fktn-k.github.io/fkYAML/api/basic_node/get_value/) for more details.  
Other changes are related to minor bug fixes in YAML scalar parsing and maintenance fixes for CI.  

### What's Changed

#### :sparkles: New Features
- Numeric scalar conversions inside basic\_node::get\_value API ([\#419](https://github.com/fktn-k/fkYAML/pull/419), [fktn-k](https://github.com/fktn-k))
    - Suggested by [ARessegetesStery](https://github.com/ARessegetesStery) in the issue [\#366](https://github.com/fktn-k/fkYAML/issues/366)
    - Automatic value conversions among null, boolean, integer and floating-point scalar values inside [fkyaml::basic_node::get_value()](../api/basic_node/get_value.md) API calls
- Add forward declaration header ([\#422](https://github.com/fktn-k/fkYAML/pull/422), [fktn-k](https://github.com/fktn-k))
    - Suggested by [abcminiuser](https://github.com/abcminiuser) in the discussion [\#420](https://github.com/fktn-k/fkYAML/discussions/420)
    - This new header provides the fkYAML namespace macros and forward declarations of fkYAML API classes.
    - This file is available both in the [include](https://github.com/fktn-k/fkYAML/tree/v0.3.14/include) and [single_include](https://github.com/fktn-k/fkYAML/tree/v0.3.14/single_include) directories.

#### :zap: Improvements
- Support more STL types in from\_node ([\#421](https://github.com/fktn-k/fkYAML/pull/421), [fktn-k](https://github.com/fktn-k))
    - YAML node objects can now be converted into a lot more STL container types.
    - See [the API documentation page](https://fktn-k.github.io/fkYAML/api/basic_node/get_value/) for more details.
- Clarify type restrictions of get\_value\(\) & get\_value\_ref\(\) APIs ([\#424](https://github.com/fktn-k/fkYAML/pull/424), [fktn-k](https://github.com/fktn-k))
    - get_value calls with unsupported types (reference, pointer and C-style array types) now emits an explicit error message.
- Use std::unreachable\(\) or similar compiler specific extensions for dead but necessary code ([\#425](https://github.com/fktn-k/fkYAML/pull/425), [fktn-k](https://github.com/fktn-k))
    - Exclude detail::unreachable\(\) from coverage target ([\#426](https://github.com/fktn-k/fkYAML/pull/426), [fktn-k](https://github.com/fktn-k))

#### :bug: Bug Fixes
- Fix wrong scalar value type detection from floating point value token ([\#414](https://github.com/fktn-k/fkYAML/pull/414), [fktn-k](https://github.com/fktn-k))
    - reported by [ebertolazzi](https://github.com/ebertolazzi) in the issue [\#413](https://github.com/fktn-k/fkYAML/issues/413)
- Disable -Wdeprecated-literal-operator warnings ([\#417](https://github.com/fktn-k/fkYAML/pull/417), [fktn-k](https://github.com/fktn-k))
    - reported by [ebertolazzi](https://github.com/ebertolazzi) in the issue [\#413](https://github.com/fktn-k/fkYAML/issues/413)

#### :robot: CI
- Update GA workflow jobs for ubuntu24.04 ([\#415](https://github.com/fktn-k/fkYAML/pull/415), [fktn-k](https://github.com/fktn-k))
- GA workflow jobs with more AppleClang versions on macOS ([\#416](https://github.com/fktn-k/fkYAML/pull/416), [fktn-k](https://github.com/fktn-k))
- Fix somehow shadowed clang-tidy warnings ([\#418](https://github.com/fktn-k/fkYAML/pull/418), [fktn-k](https://github.com/fktn-k))
- Fix duplicate include error if clang-tidy helper file already exists ([\#423](https://github.com/fktn-k/fkYAML/pull/423), [fktn-k](https://github.com/fktn-k))

### Full Changelog
https://github.com/fktn-k/fkYAML/compare/v0.3.13...v0.3.14

---

## **fkYAML version 0.3.13**

!!! abstract "Release Packages"

    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.13/fkYAML.zip)
    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.13/fkYAML.tgz)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.13/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.13/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.13/node.hpp) (single header)

### Summary

This release includes performance improvements in parsing YAML documents and many bug fixes in both serialization and deserialization functions.  
In addition, benchmarking has been improved to facilitate comparison with existing YAML libraries by adding variations in data file content.  
No changes are required for migration.  

### What's Changed

#### :zap: Improvements
- Refine benchmarking ([\#397](https://github.com/fktn-k/fkYAML/pull/397), [fktn-k](https://github.com/fktn-k))
    - replaced data files for benchmarking for better comparison against existing YAML libraries
- Optimized scalar parsing ([\#409](https://github.com/fktn-k/fkYAML/pull/409), [fktn-k](https://github.com/fktn-k))
    - increased parse speed by about 5MiB/s

#### :bug: Bug Fixes
- Accept % as first scalar character ([\#399](https://github.com/fktn-k/fkYAML/pull/399), [fktn-k](https://github.com/fktn-k))
- Fix compile warnings/errors when benchmarker app is compiled with msvc ([\#401](https://github.com/fktn-k/fkYAML/pull/401), [fktn-k](https://github.com/fktn-k))
- Updated natvis file ([\#402](https://github.com/fktn-k/fkYAML/pull/402), [fktn-k](https://github.com/fktn-k))
    - fixed broken natvis file contents
- Fix URI validation for tag shorthands ([\#403](https://github.com/fktn-k/fkYAML/pull/403), [fktn-k](https://github.com/fktn-k))
- Fix float scalar serialization when a float is actually an integer ([\#407](https://github.com/fktn-k/fkYAML/pull/407), [fktn-k](https://github.com/fktn-k))
    - reported by [ARessegetesStery](https://github.com/ARessegetesStery) in the issue [\#405](https://github.com/fktn-k/fkYAML/issues/405)
- Fix infinite loops after parsing final empty block scalar ([\#410](https://github.com/fktn-k/fkYAML/pull/410), [fktn-k](https://github.com/fktn-k))
- Fix wrong parse result from single scalar document ([\#411](https://github.com/fktn-k/fkYAML/pull/411), [fktn-k](https://github.com/fktn-k))

#### :memo: Documentation
- Remove header lines for Doxygen ([\#398](https://github.com/fktn-k/fkYAML/pull/398), [fktn-k](https://github.com/fktn-k))
- Fix some typos in strings and comments ([\#408](https://github.com/fktn-k/fkYAML/pull/408), [sndth](https://github.com/sndth))

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.3.12...v0.3.13

---

## **fkYAML version 0.3.12**

!!! abstract "Release Packages"

    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.12/fkYAML.zip)
    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.12/fkYAML.tgz)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.12/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.12/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.12/node.hpp) (single header)

### Summary

This release adds some basic_node APIs to get/set node attributes for more secure backwards compatibilities in future releases. Because of that, some existing APIs have been deprecated (see the list down below). It's recommended for users to replace deprecated API usages with new APIs.  
Furthermore, relatively large refactoring has been made for better performance and more flexible node attribute configurations (just preparation for now, but will be added within a few coming releases). As a result, deserialization performance has increased. See the Benchmarking section in README.md for details.  
Last but not least, several bugs have also been resolved in deserialization, and active C++ standard is now correctly detected at compile time when using MSVC compilers where `__cplusplus` macro doesn't hold a correct value by default.  

#### :sparkles: New Features

- Add node\_type/yaml\_version\_type enum class APIs by [fktn-k](https://github.com/fktn-k) in [\#388](https://github.com/fktn-k/fkYAML/pull/388)
    - In this PR, the following APIs has been deprecated. Although they still work as before except for compile-time deprecation warnings, it's highly recommended to replace their usages with new APIs since they are planned to be removed in v0.4.0. See the Deprecation notes in each deprecated API reference page for migration guides.
        - [`fkyaml::basic_node::node_t`](https://fktn-k.github.io/fkYAML/api/basic_node/node_t/)  
            - Replace with: [`fkyaml::node_type`](https://fktn-k.github.io/fkYAML/api/node_type/)
        - [`fkyaml::basic_node::basic_node(const fkyaml::basic_node::node_t)`](https://fktn-k.github.io/fkYAML/api/basic_node/constructor/#overload-3)
            - Replace with: [`fkyaml::basic_node::basic_node(const fkyaml::node_type)`](https://fktn-k.github.io/fkYAML/api/basic_node/constructor/#overload-2)
        - [`fkyaml::basic_node::node_t fkyaml::basic_node::type()`](https://fktn-k.github.io/fkYAML/api/basic_node/type/)
            - Replace with: [`fkyaml::basic_node::get_type()`](https://fktn-k.github.io/fkYAML/api/basic_node/get_type/)
        - [`fkyaml::basic_node::yaml_version_t`](https://fktn-k.github.io/fkYAML/api/basic_node/yaml_version_t/)
            - Replace with: [`fkyaml::yaml_version_type`](https://fktn-k.github.io/fkYAML/api/yaml_version_type/)
        - [`fkyaml::basic_node::yaml_version_t fkyaml::basic_node::get_yaml_version()`](https://fktn-k.github.io/fkYAML/api/basic_node/get_yaml_version/)
            - Replace with: [`fkyaml::yaml_version_type fkyaml::basic_node::get_yaml_version_type()`](https://fktn-k.github.io/fkYAML/api/basic_node/get_yaml_version_type/)
        - [`void fkyaml::basic_node::set_yaml_version(const fkyaml::basic_node::yaml_version_t)`](https://fktn-k.github.io/fkYAML/api/basic_node/set_yaml_version/)
            - Replace with: [`void fkyaml::basic_node::set_yaml_version_type(const fkyaml::yaml_version_type)`](https://fktn-k.github.io/fkYAML/api/basic_node/set_yaml_version_type/)


#### :zap: Improvements

- Improve UTF encoding detection by [fktn-k](https://github.com/fktn-k) in [\#386](https://github.com/fktn-k/fkYAML/pull/386)
- Refactor node attributes management by [fktn-k](https://github.com/fktn-k) in [\#389](https://github.com/fktn-k/fkYAML/pull/389)
- Refactor lexical analysis by [fktn-k](https://github.com/fktn-k) in [\#390](https://github.com/fktn-k/fkYAML/pull/390)
- Improve conversion from scalars to native types by [fktn-k](https://github.com/fktn-k) in [\#391](https://github.com/fktn-k/fkYAML/pull/391)
- Reduce string copies in parse by [fktn-k](https://github.com/fktn-k) in [\#394](https://github.com/fktn-k/fkYAML/pull/394)
- Use likely/unlikely if available by [fktn-k](https://github.com/fktn-k) in [\#395](https://github.com/fktn-k/fkYAML/pull/395)

#### :bug: Bug Fixes

- Fix detecting invalid contents of block scalar headers by [fktn-k](https://github.com/fktn-k) in [\#387](https://github.com/fktn-k/fkYAML/pull/387)
- Use \_MSVC\_LANG macro when compiled with MSVC for C++ standard detection by [fktn-k](https://github.com/fktn-k) in [\#392](https://github.com/fktn-k/fkYAML/pull/392)
- Parse +.inf as the positive float infinity by [fktn-k](https://github.com/fktn-k) in [\#393](https://github.com/fktn-k/fkYAML/pull/393)


**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.3.11...v0.3.12

---

## **fkYAML version 0.3.11**

!!! abstract "Release Packages"

    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.11/fkYAML.zip)
    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.11/fkYAML.tgz)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.11/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.11/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.11/node.hpp) (single header)

### Summary

This release fixes bugs in the deserialization feature including input handling.  
Furthermore, CI workflow errors when a PR from fork repository tries to acquire write permission/secrets access are also resolved, which was introduced when code formatting & coverage report sharing were made executed during the GitHub Actions workflows running for PR reviews.  

### What's Changed

#### :bug: Bug Fixes

- Allow the document end marker omitted by [fktn-k](https://github.com/fktn-k) in [\#374](https://github.com/fktn-k/fkYAML/pull/374)
- \#375 Fix newline code normalization in UTF-8 encoded file input by [fktn-k](https://github.com/fktn-k) in [\#376](https://github.com/fktn-k/fkYAML/pull/376), reported by [sndth](https://github.com/sndth) in [\#375](https://github.com/fktn-k/fkYAML/issues/375)
- Fix wrong parse result from a block sequence as a nested block mapping value by [fktn-k](https://github.com/fktn-k) in [\#377](https://github.com/fktn-k/fkYAML/pull/377)
- Throw exception when loading a nonexistent stream by [sndth](https://github.com/sndth) in [\#379](https://github.com/fktn-k/fkYAML/pull/379), reported by [sndth](https://github.com/sndth) in [\#378](https://github.com/fktn-k/fkYAML/issues/378)
- Fix error on parsing a floating point value which ends with a dot by [fktn-k](https://github.com/fktn-k) in [\#382](https://github.com/fktn-k/fkYAML/pull/382), reported by [realysy](https://github.com/realysy) in [\#380](https://github.com/fktn-k/fkYAML/issues/380)

#### :robot: CI

- Fix workflow errors on PRs from fork repos by [fktn-k](https://github.com/fktn-k) in [\#383](https://github.com/fktn-k/fkYAML/pull/383)
- Fix conditional branching for pull\_request\_target events by [fktn-k](https://github.com/fktn-k) in [\#384](https://github.com/fktn-k/fkYAML/pull/384)

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.3.10...v0.3.11

---

## **fkYAML version 0.3.10**

!!! abstract "Release Packages"

    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.10/fkYAML.zip)
    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.10/fkYAML.tgz)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.10/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.10/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.10/node.hpp) (single header)

### Summary

This release adds the new support for deserializing/serializing multiple YAML documents by adding new APIs ([`fkyaml::node::deserialize_docs()`](https://fktn-k.github.io/fkYAML/api/basic_node/deserialize_docs/) and [`fkyaml::node::serialize_docs()`](https://fktn-k.github.io/fkYAML/api/basic_node/serialize_docs/) respectively). You can still call the existing APIs for deserializing/serializing a single YAML document. See the linked API document pages for details.  
In addition, from this version on, Intel icpx compiler is supported and used in the CI processes. Some compiler flags are added in building the unit testing app, but no compiler specific switches are required for the library itself.  
Moreover, because the GitHub Actions runner image for macOS11 has been deprecated, the CI jobs which uses the runner image and some compiler support which depend on the runner image have been removed.  
For other changes like bug fixes, see descriptions in each related issues and PRs.  

### What's Changed

#### :sparkles: New Features

- Support Intel icpx compiler by [fktn-k](https://github.com/fktn-k) in [\#360](https://github.com/fktn-k/fkYAML/pull/360)
- Support parssing multiple YAML documents by [fktn-k](https://github.com/fktn-k) in [\#362](https://github.com/fktn-k/fkYAML/pull/362)
- Support serializing multiple YAML docs by [fktn-k](https://github.com/fktn-k) in [\#363](https://github.com/fktn-k/fkYAML/pull/363)

#### :bug: Bug Fixes

- Detect missing the end of directives markers (---) by [fktn-k](https://github.com/fktn-k) in [\#361](https://github.com/fktn-k/fkYAML/pull/361)
- Fix bug in serializing alias keys by [fktn-k](https://github.com/fktn-k) in [\#364](https://github.com/fktn-k/fkYAML/pull/364)
- Allow trailing comma in flow mapping/sequence by [fktn-k](https://github.com/fktn-k) in [\#365](https://github.com/fktn-k/fkYAML/pull/365)
- \#368 Fix line advancement after node props by [fktn-k](https://github.com/fktn-k) in [\#369](https://github.com/fktn-k/fkYAML/pull/369), reported by [alienczf](https://github.com/alienczf) in [\#368](https://github.com/fktn-k/fkYAML/issues/368)
- Apply node properties for the root node by [fktn-k](https://github.com/fktn-k) in [\#370](https://github.com/fktn-k/fkYAML/pull/370)
- \#366 Fix node-to-float conversion error if node's value is \<= 0 by [fktn-k](https://github.com/fktn-k) in [\#371](https://github.com/fktn-k/fkYAML/pull/371), reported by [ARessegetesStery](https://github.com/ARessegetesStery) in [\#366](https://github.com/fktn-k/fkYAML/issues/366)

#### :robot: CI

- Remove CI jobs running with macOS 11 by [fktn-k](https://github.com/fktn-k) in [\#367](https://github.com/fktn-k/fkYAML/pull/367)
- update configs for reuse v4 by [fktn-k](https://github.com/fktn-k) in [\#372](https://github.com/fktn-k/fkYAML/pull/372)

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.3.9...v0.3.10

---

## **fkYAML version 0.3.9**

!!! abstract "Release Packages"

    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.9/fkYAML.zip)
    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.9/fkYAML.tgz)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.9/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.9/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.9/node.hpp) (single header)

### Summary

This release adds the benchmarking tool for fkYAML and other C++ libraries for YAML.  
The tool is quite experimental and will be modified and expanded in future releases.  
See the benchmarking section of the README.md file for the current benchmarking scores.  
Moreover, several bugs in deserialization and Windows builds have been fixed.  

### What's Changed

#### :sparkles: New Features

- Add benchmarking tool by [fktn-k](https://github.com/fktn-k) in [\#354](https://github.com/fktn-k/fkYAML/pull/354)

#### :zap: Improvements

- Refactor for optimization by [fktn-k](https://github.com/fktn-k) in [\#356](https://github.com/fktn-k/fkYAML/pull/356)

#### :bug: Bug Fixes

- fixed parse error on explicit mappings as a block sequence entry by [fktn-k](https://github.com/fktn-k) in [\#353](https://github.com/fktn-k/fkYAML/pull/353)
- Fix parse error on small negative integer keys by [fktn-k](https://github.com/fktn-k) in [\#355](https://github.com/fktn-k/fkYAML/pull/355)
- Fix compile errors on Windows by [fktn-k](https://github.com/fktn-k) in [\#358](https://github.com/fktn-k/fkYAML/pull/358), reported by [Michael-Lafreniere](https://github.com/Michael-Lafreniere) in [\#357](https://github.com/fktn-k/fkYAML/issues/357)

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.3.8...v0.3.9

---

## **fkYAML version 0.3.8**

!!! abstract "Release Packages"

    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.8/fkYAML.zip)
    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.8/fkYAML.tgz)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.8/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.8/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.8/node.hpp) (single header)

### Summary

This release contains the new support of multiline scalars in the deserialization feature.  
As described in the PR [\#344](https://github.com/fktn-k/fkYAML/pull/344), the support is limited to single/double quoted styles and multiline scalar keys are being permitted as opposed to the YAML specification.  
The above limitations will, of course, be resolved in future releases.  
Furthermore, a number of bugs in the deserialization feature have been resolved as shown in the Bug Fixes section down below.  
Last but not least, gcc14 and clang18 have been added to the supported compilers list and are now used in the GitHub Actions workflows for testing.

### What's Changed

#### :sparkles: New Features

- support flow line folding for single/double quoted scalars by [fktn-k](https://github.com/fktn-k) in [\#344](https://github.com/fktn-k/fkYAML/pull/344)

#### :zap: Improvements

- Normalize newline codes in the input buffer by [fktn-k](https://github.com/fktn-k) in [\#343](https://github.com/fktn-k/fkYAML/pull/343)

#### :bug: Bug Fixes

- Fix parse error on flow containers containing child flow containers by [fktn-k](https://github.com/fktn-k) in [\#345](https://github.com/fktn-k/fkYAML/pull/345)
- Fix parse error on flow container keys by [fktn-k](https://github.com/fktn-k) in [\#346](https://github.com/fktn-k/fkYAML/pull/346)
- Fix wrong parse result from mappings containing the non-first block sequence by [fktn-k](https://github.com/fktn-k) in [\#348](https://github.com/fktn-k/fkYAML/pull/348)
- Detect errors on missing flow value separators by [fktn-k](https://github.com/fktn-k) in [\#350](https://github.com/fktn-k/fkYAML/pull/350)
- Fix parse error on empty flow containers by [fktn-k](https://github.com/fktn-k) in [\#351](https://github.com/fktn-k/fkYAML/pull/351)

#### :robot: CI

- added gcc14 & clang18 to the compiler version list for testing by [fktn-k](https://github.com/fktn-k) in [\#349](https://github.com/fktn-k/fkYAML/pull/349)

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.3.7...v0.3.8

---

## **fkYAML version 0.3.7**

!!! abstract "Release Packages"

    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.7/fkYAML.zip)
    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.7/fkYAML.tgz)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.7/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.7/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.7/node.hpp) (single header)

### Summary

This release includes refactoring of YAML escaping and improvement of the way of managing anchor/alias nodes.  
Furthermore, several bugs found in the deserializer are resolved.  

For contributors, the results of executing clang-format and amalgamation in the GitHub Actions workflows are now automatically applied to the source files.  
You can, of course, still run those tools locally but that's not required from this release on.  
For more details, see the CONTRIBUTING.md file.  

### What's Changed

#### :zap: Improvements

- Separate YAML escaping/unescaping functionalities by [@fktn-k](https://github.com/fktn-k) in [\#337](https://github.com/fktn-k/fkYAML/pull/337)
- Improve anchor alias node management by [@fktn-k](https://github.com/fktn-k) in [\#340](https://github.com/fktn-k/fkYAML/pull/340)

#### :bug: Bug Fixes

- Fix false error on anchor names containing colons \(:\) by [@fktn-k](https://github.com/fktn-k) in [\#335](https://github.com/fktn-k/fkYAML/pull/335)
- Escape backslash(\\) + 0x09 to a horizontal tab (\t) by [@fktn-k](https://github.com/fktn-k) in [\#336](https://github.com/fktn-k/fkYAML/pull/336)
- Fixed error on node properties for child block sequences by [@fktn-k](https://github.com/fktn-k) in [\#338](https://github.com/fktn-k/fkYAML/pull/338)
- Fix the C6262 warning on Windows by [@fktn-k](https://github.com/fktn-k) in [\#339](https://github.com/fktn-k/fkYAML/pull/339)

#### :robot: CI

- Run and apply the result of clang-format & amalagamation in GA workflows by [@fktn-k](https://github.com/fktn-k) in [\#341](https://github.com/fktn-k/fkYAML/pull/341)

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.3.6...v0.3.7

---

## **fkYAML version 0.3.6**

!!! abstract "Release Packages"

    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.6/fkYAML.zip)
    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.6/fkYAML.tgz)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.6/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.6/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.6/node.hpp) (single header)

### Summary

This release includes enhancements to the deserializer, improving both performance and readability.  
Furthermore, it resolves several issues identified in the deserializer and improves conformance to the YAML specification.  

### What's Changed

#### :zap: Improvements

- Refactor deserialization process by [@fktn-k](https://github.com/fktn-k) in [\#331](https://github.com/fktn-k/fkYAML/pull/331)

#### :bug: Bug Fixes

- Fixed parser crashes on a sequence right after the directives end marker by [@fktn-k](https://github.com/fktn-k) in [\#327](https://github.com/fktn-k/fkYAML/pull/327)
- Allow backslashes in plain/single-quoted scalars by [@fktn-k](https://github.com/fktn-k) in [\#329](https://github.com/fktn-k/fkYAML/pull/329)
- Fixed parse error on root block sequence with child block sequences by [@fktn-k](https://github.com/fktn-k) in [\#330](https://github.com/fktn-k/fkYAML/pull/330)
- Fix parse error on block sequences with child flow style container nodes by [@fktn-k](https://github.com/fktn-k) in [\#332](https://github.com/fktn-k/fkYAML/pull/332)
- Fixed parse error on block sequences with child block mappings split by a newline code by [@fktn-k](https://github.com/fktn-k) in [\#333](https://github.com/fktn-k/fkYAML/pull/333)

#### :people_holding_hands: Community

- Fixed no such file/directory error when running Bash scripts on Windows by [@fktn-k](https://github.com/fktn-k) in [\#328](https://github.com/fktn-k/fkYAML/pull/328)

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.3.5...v0.3.6

---

## **fkYAML version 0.3.5**

!!! abstract "Release Packages"

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.5/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.5/fkYAML.zip)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.5/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.5/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.5/node.hpp) (single header)

### Summary

This release adds the new support for serialization of YAML container key nodes either with or without tags and/or anchors.  
In addition, Win32 builds are now tested during the GitHub Actions workflows.
Moreover, the performance and efficiency of Unicode handlings have been improved by refactoring the deserialization feature.  

For contributors, fkYAML now depends on the Clang-Format Python distribution as the source code formatter and provides Bash/Batch scripts for running the Clang-Format and amalgamation.  
For more information, see the latest CONTRIBUTING.md file.  

### What's Changed

#### :sparkles: New Features

- Serialize container keys by [@fktn-k](https://github.com/fktn-k) in [\#319](https://github.com/fktn-k/fkYAML/pull/319)

#### :zap: Improvements

- Query the count of UTF-8 character bytes by [@fktn-k](https://github.com/fktn-k) in [\#317](https://github.com/fktn-k/fkYAML/pull/317)
- Improved UTF encoding processing by [@fktn-k](https://github.com/fktn-k) in [\#318](https://github.com/fktn-k/fkYAML/pull/318)
- Reformat source files by [@fktn-k](https://github.com/fktn-k) in [\#320](https://github.com/fktn-k/fkYAML/pull/320)

#### :bug: Bug Fixes

- Fix parse error on the first node which isn't a block mapping by [@fktn-k](https://github.com/fktn-k) in [\#322](https://github.com/fktn-k/fkYAML/pull/322)
- Force LF newline codes in the amalgamated file by [@fktn-k](https://github.com/fktn-k) in [\#324](https://github.com/fktn-k/fkYAML/pull/324)

#### :robot: CI

- Add workflows items to test Win32 builds by [@fktn-k](https://github.com/fktn-k) in [\#323](https://github.com/fktn-k/fkYAML/pull/323)

#### :people_holding_hands: Community

- Use Clang-Format python distribution by [@fktn-k](https://github.com/fktn-k) in [\#316](https://github.com/fktn-k/fkYAML/pull/316)
- Set the unit test app as the Visual Studio startup project by [@fktn-k](https://github.com/fktn-k) in [\#321](https://github.com/fktn-k/fkYAML/pull/321)
- Add Bash & Batch scripts to run clang-format & amalgamation by [@fktn-k](https://github.com/fktn-k) in [\#325](https://github.com/fktn-k/fkYAML/pull/325)



**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.3.4...v0.3.5

---

## **fkYAML version 0.3.4**

!!! abstract "Release Packages"

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.4/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.4/fkYAML.zip)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.4/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.4/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.4/node.hpp) (single header)

### Summary

This release adds the new support for YAML tags in both the deserialization and serialization features while improving the performance and efficiency of the deserialization feature.  
In addition, std::basic_string_view types, such as std::string_view, are now explicitly supported, and implementations of conversion from string scalar nodes and test cases for the functions that take those types as arguments.  
For CI processes, workflows using the macOS 14 GitHub Actions runner image have been added, and platform support for fkYAML has been extended.  

### What's Changed

#### :sparkles: New Features

- \#181 support tag for scalars by [fktn-k](https://github.com/fktn-k) in [\#305](https://github.com/fktn-k/fkYAML/pull/305)
- support tags for container nodes with some cleanup in test suite by [fktn-k](https://github.com/fktn-k) in [\#309](https://github.com/fktn-k/fkYAML/pull/309)
- Add workflow jobs with the macOS14 runner image by [fktn-k](https://github.com/fktn-k) in [\#310](https://github.com/fktn-k/fkYAML/pull/310)
- \#294 Support std::basic\_string\_view by [fktn-k](https://github.com/fktn-k) in [\#313](https://github.com/fktn-k/fkYAML/pull/313)
- \#181 Serialize container tags by [fktn-k](https://github.com/fktn-k) in [\#314](https://github.com/fktn-k/fkYAML/pull/314)

#### :zap: Improvements

- Scan inputs using iterators by [fktn-k](https://github.com/fktn-k) in [\#306](https://github.com/fktn-k/fkYAML/pull/306)
- Defer scalar conversions by [fktn-k](https://github.com/fktn-k) in [\#308](https://github.com/fktn-k/fkYAML/pull/308)

#### :robot: CI

- Fix Valgrind error in the test suite by [fktn-k](https://github.com/fktn-k) in [\#307](https://github.com/fktn-k/fkYAML/pull/307)
- Organized the contents of the test suite by [fktn-k](https://github.com/fktn-k) in [\#311](https://github.com/fktn-k/fkYAML/pull/311)

#### :memo: Documentation

- updated dependencies for mkdocs to the latest versions by [fktn-k](https://github.com/fktn-k) in [\#312](https://github.com/fktn-k/fkYAML/pull/312)

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.3.3...v0.3.4

---

## **fkYAML version 0.3.3**

!!! abstract "Release Packages"

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.3/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.3/fkYAML.zip)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.3/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.3/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.3/node.hpp) (single header)

### Summary

This release adds a new basic_node API, at(), which validates before either (1) that the specified key exists in the mapping or (2) that the specified index is less than the current size of the sequence before accessing the target node.  
Several bugs bugs in the deserialization feature have also been fixed.  
In addition, refactoring of the deserializer has improved the handling of input characters.  

### What's Changed

#### :sparkles: New Features

- \#298 Add at\(\) API to the basic\_node class by [fktn-k](https://github.com/fktn-k) in [\#299](https://github.com/fktn-k/fkYAML/pull/299)

#### :zap: Improvements

- Further improvements of input handlings by [fktn-k](https://github.com/fktn-k) in [\#301](https://github.com/fktn-k/fkYAML/pull/301)
- Fixed warnings and made future warnings as errors by [fktn-k](https://github.com/fktn-k) in [\#300](https://github.com/fktn-k/fkYAML/pull/300)
- Improve handling UTF encoded inputs by [fktn-k](https://github.com/fktn-k) in [\#296](https://github.com/fktn-k/fkYAML/pull/296)
- modied the way of formatting error messages for exception objects by [fktn-k](https://github.com/fktn-k) in [\#291](https://github.com/fktn-k/fkYAML/pull/291)

#### :bug: Bug Fixes

- \#302 Fixed parse error on alias mapping keys [\#303](https://github.com/fktn-k/fkYAML/pull/303) [fktn-k](https://github.com/fktn-k)
- \#292 Better handling for flow indicators in permitted scalar contexts by [stephenwhittle](https://github.com/stephenwhittle) in [\#293](https://github.com/fktn-k/fkYAML/pull/293)
- \#288 Fixed incorrect parse results from mapping entries split across newlines by [fktn-k](https://github.com/fktn-k) in [\#289](https://github.com/fktn-k/fkYAML/pull/289) (reported by [stephenwhittle](https://github.com/stephenwhittle))

#### :robot: CI

- Resolve warnings against using Node.js 16 by [fktn-k](https://github.com/fktn-k) in [\#290](https://github.com/fktn-k/fkYAML/pull/290)

#### :people_holding_hands: Community

- \#297 Add a note for checking the coverage & upload the same as an artifact by [fktn-k](https://github.com/fktn-k) in [\#295](https://github.com/fktn-k/fkYAML/pull/295) (suggested by [stephenwhittle](https://github.com/stephenwhittle))
- Add .editorconfig file by [fktn-k](https://github.com/fktn-k) in [\#287](https://github.com/fktn-k/fkYAML/pull/287)

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.3.2...v0.3.3

---

## **fkYAML version 0.3.2**

!!! abstract "Release Packages"

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.2/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.2/fkYAML.zip)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.2/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.2/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.2/node.hpp) (single header)

### Summary

This release contains the `char8_t` support for deserialization and bug fixes related to the deserialization feature.  
Furthermore, the list of the supported compilers are expanded since the GitHub Actions runner image `macOS-13` has been officially released.  

### What's Changed

#### :sparkles: New Features

- \#237 Support char8\_t in deserialization [\#269](https://github.com/fktn-k/fkYAML/pull/269) ([fktn-k](https://github.com/fktn-k))

#### :zap: Improvements

- Clean up CMakeLists files [\#280](https://github.com/fktn-k/fkYAML/pull/280) ([fktn-k](https://github.com/fktn-k))
- Modify handling node properties [\#270](https://github.com/fktn-k/fkYAML/pull/270) ([fktn-k](https://github.com/fktn-k))

#### :bug: Bug Fixes

- \#281 Fixed the parser crash due to comments right after a sequence block key [\#284](https://github.com/fktn-k/fkYAML/pull/284) ([fktn-k](https://github.com/fktn-k))
- \#282 Don't traverse up to the parent node immediately after parsing a flow-mapping value [\#283](https://github.com/fktn-k/fkYAML/pull/283) ([stephenwhittle](https://github.com/stephenwhittle))
- \#277 Fixed incorrect parse result from plain scalars starting with special values [\#278](https://github.com/fktn-k/fkYAML/pull/278) ([fktn-k](https://github.com/fktn-k))
- \#275 Fixed parse error on plain scalars containing flow indicators [\#276](https://github.com/fktn-k/fkYAML/pull/276) ([fktn-k](https://github.com/fktn-k))
- \#272 Fix parse error on a block sequence containing a comment within [\#273](https://github.com/fktn-k/fkYAML/pull/273) ([fktn-k](https://github.com/fktn-k))

#### :robot: CI

- Add workflow jobs with the macos-13 runner image [\#274](https://github.com/fktn-k/fkYAML/pull/274) ([fktn-k](https://github.com/fktn-k))

#### :memo: Documentation

- Updated documents [\#279](https://github.com/fktn-k/fkYAML/pull/279) ([fktn-k](https://github.com/fktn-k))
- Updated copyright year [\#271](https://github.com/fktn-k/fkYAML/pull/271) ([fktn-k](https://github.com/fktn-k))

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.3.1...v0.3.2

---

## **fkYAML version 0.3.1**

!!! abstract "Release Packages"

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.1/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.1/fkYAML.zip)
    * [fkYAML_single_header.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.3/fkYAML_single_header.zip)
    * [fkYAML_single_header.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.3/fkYAML_single_header.tgz)
    * [node.hpp](https://github.com/fktn-k/fkYAML/releases/download/v0.3.1/node.hpp) (single header)

### Summary

From this release on, fkYAML provides the amalgamated version to improve portability as a header-only library.  
Also, this release contains bug-fixes in the deserialization process.  
Furthermore, the documentation now uses outputs from the sources in `docs/examples` directory to keep the examples in the documentation up to date with the latest library sources.  

### What's Changed

#### :sparkles: New Features

- \#255 serve single header file [\#261](https://github.com/fktn-k/fkYAML/pull/261) ([fktn-k](https://github.com/fktn-k)) suggested by [@reFX-Mike](https://github.com/reFX-Mike)

#### :zap: Improvements

- \#256 Added specialization of from\_node\(\) for std::map with compatible key/value types [\#260](https://github.com/fktn-k/fkYAML/pull/260) ([fktn-k](https://github.com/fktn-k))

#### :bug: Bug Fixes

- Avoid using unintended C++14 features [\#266](https://github.com/fktn-k/fkYAML/pull/266) ([fktn-k](https://github.com/fktn-k)) reported by [@burgreen](https://github.com/burgreen)
- Allow a dash\(-\) at the first character of a plain scalar [\#254](https://github.com/fktn-k/fkYAML/pull/254) ([fktn-k](https://github.com/fktn-k))
- Allow double quotation marks in plain scalars [\#253](https://github.com/fktn-k/fkYAML/pull/253) ([fktn-k](https://github.com/fktn-k))
- \#242 Fix error in parsing string consisting only of numbers and multiple dots [\#251](https://github.com/fktn-k/fkYAML/pull/251) ([fktn-k](https://github.com/fktn-k))

#### :robot: CI

- Added paths filter to regulate workflow executions [\#263](https://github.com/fktn-k/fkYAML/pull/263) ([fktn-k](https://github.com/fktn-k))

#### :memo: Documentation

- fixed typos which needs to be target\_link\_libraries\(\) in tutorials [\#267](https://github.com/fktn-k/fkYAML/pull/267) ([fktn-k](https://github.com/fktn-k)) reported by [@burgreen](https://github.com/burgreen)
- \#229 Fix the documentation build errors [\#265](https://github.com/fktn-k/fkYAML/pull/265) ([fktn-k](https://github.com/fktn-k))
- \#229 build example code and use output [\#264](https://github.com/fktn-k/fkYAML/pull/264) ([fktn-k](https://github.com/fktn-k))

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.3.0...v0.3.1

---

## **fkYAML version 0.3.0**

!!! abstract "Release Packages"

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.3.0/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.3.0/fkYAML.zip)

### Summary
This release contains some breaking changes in the `basic_node`'s mapping node type so that fkYAML supports non-string-scalar nodes, such as mappings or integer scalars, as mapping keys.  
Furthermore, from this release on, GCC-7 and GCC-8 are in the list of supported compilers and used for testing on GitHub Actions.  

### What's Changed

#### :boom: Breaking Changes

- \#240 Support explicit block mappings & non-scalar nodes as mapping keys [\#245](https://github.com/fktn-k/fkYAML/pull/245) ([fktn-k](https://github.com/fktn-k))
- \#240 Support non-string scalar nodes as mapping keys [\#244](https://github.com/fktn-k/fkYAML/pull/244) ([fktn-k](https://github.com/fktn-k))
- Feature/238 apply correct noexcept [\#241](https://github.com/fktn-k/fkYAML/pull/241) ([fktn-k](https://github.com/fktn-k))

#### :zap: Improvements
- \#240 Support non-string-scalar node keys in basic\_node ctor with std::initializer\_list [\#247](https://github.com/fktn-k/fkYAML/pull/247) ([fktn-k](https://github.com/fktn-k))
- \#240 implement node comparison [\#243](https://github.com/fktn-k/fkYAML/pull/243) ([fktn-k](https://github.com/fktn-k))

#### :bug: Bug Fixes
- Fixed missing callings of `fclose()` in the unit tests [\#246](https://github.com/fktn-k/fkYAML/pull/246) ([fktn-k](https://github.com/fktn-k))

#### :robot: CI
- Add GCC compiler versions tested in GA workflows [\#248](https://github.com/fktn-k/fkYAML/pull/248) ([fktn-k](https://github.com/fktn-k))

#### :memo: Documentation
- Update copyright description [\#249](https://github.com/fktn-k/fkYAML/pull/249) ([fktn-k](https://github.com/fktn-k))

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.2.3...v0.3.0

---

## **fkYAML version 0.2.3**

!!! abstract "Release Packages"

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.2.3/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.2.3/fkYAML.zip)

### Summary

This release contains support for the deserialization of YAML block style scalars and the user-defined string literals for YAML node objects.  
A bug in parsing UTF-16 encoded inputs has also been fixed in this release.  
Furthermore, from this release on, natvis debugger visualization file is/will be contained when fkYAML is built with the MSVC environments for better debugging experience.  

### What's Changed

#### :zap: Improvements
- Feature/232 user defined literal for deserialization [\#235](https://github.com/fktn-k/fkYAML/pull/235) ([fktn-k](https://github.com/fktn-k))
- \#159 Support block scalar styles [\#228](https://github.com/fktn-k/fkYAML/pull/228) ([fktn-k](https://github.com/fktn-k))
- Feature/226 add natvis file [\#227](https://github.com/fktn-k/fkYAML/pull/227) ([fktn-k](https://github.com/fktn-k))

#### :bug: Bug Fixes
- fixed parsing UTF-16 encoded inputs which do not end with a surrogate… [\#234](https://github.com/fktn-k/fkYAML/pull/234) ([fktn-k](https://github.com/fktn-k))

#### :memo: Documentation
- Improved contribution guide [\#231](https://github.com/fktn-k/fkYAML/pull/231) ([fktn-k](https://github.com/fktn-k))
- Removed dependency on the cmake-format tool [\#230](https://github.com/fktn-k/fkYAML/pull/230) ([fktn-k](https://github.com/fktn-k))
- Fixed wrong URLs in the releases page of the documentation [\#225](https://github.com/fktn-k/fkYAML/pull/225) ([fktn-k](https://github.com/fktn-k))

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.2.2...v0.2.3

---

## **fkYAML version 0.2.2**

!!! abstract "Release Packages"

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.2.2/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.2.2/fkYAML.zip)

### Summary

This release supports the UTF-8, UTF-16BE/LE, and UTF-32BE/LE encodings including BOMs in parsing YAML documents, and improved the contents of error messages by adding runtime information when exceptions are thrown.  
Furthermore, bugs in the deserialization/serialization features have also been fixed.  

!!! warning CAVEAT

    The fkYAML library is an alpha quality software, at most.  
    It would take a while until this library gets stabled version, since implementation and maintenance has been/will be done by a single developer.  
    If you find a bug or wanted features, please open a discussion/issue to let me know.  
    Any kind of contribution is very much welcomed.  

### What's Changed

#### :zap: Improvements

- \#209 Support auto-detection of Unicode encoding types including BOMs [\#223](https://github.com/fktn-k/fkYAML/pull/223) ([fktn-k](https://github.com/fktn-k))
- \#214 Emit more descriptive errors [\#222](https://github.com/fktn-k/fkYAML/pull/222) ([fktn-k](https://github.com/fktn-k))
- \#209 support UTF-16 / UTF-32 for deserialization input characters [\#221](https://github.com/fktn-k/fkYAML/pull/221) ([fktn-k](https://github.com/fktn-k))

#### :bug: Bug Fixes

- \#218 Fixed lacking quotes & escapes during serialization [\#220](https://github.com/fktn-k/fkYAML/pull/220) ([fktn-k](https://github.com/fktn-k))
- \#217 Fix parse errors on strings containing single quotes [\#219](https://github.com/fktn-k/fkYAML/pull/219) ([fktn-k](https://github.com/fktn-k))

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.2.1...v0.2.2

---

## **fkYAML version 0.2.1**

!!! abstract "Release Packages"

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.2.1/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.2.1/fkYAML.zip)

### Summary

This release supports the UTF-8 encoding (both escaped/unescaped) in parsing YAML documents.  
Furthermore, bugs in the deserialization feature have also been fixed.  

!!! warning CAVEAT

    The fkYAML library is an alpha quality software, at most.  
    It would take a while until this library gets stabled version, since implementation and maintenance has been/will be done by a single developer.  
    If you find a bug or wanted features, please open a discussion/issue to let me know.  
    Any kind of contribution is very much welcomed.  

### What's Changed

#### :zap: Improvements

- \#209 support escaped unicode [\#215](https://github.com/fktn-k/fkYAML/pull/215) ([fktn-k](https://github.com/fktn-k))
- \#209 Support unescaped UTF-8 input characters in deserialization [\#210](https://github.com/fktn-k/fkYAML/pull/210) ([fktn-k](https://github.com/fktn-k))
- \#207 Support markers for the end of directives/documents [\#208](https://github.com/fktn-k/fkYAML/pull/208) ([fktn-k](https://github.com/fktn-k))
- \#190 Support specialization of deserialization for a vector of user-defined type objects [\#203](https://github.com/fktn-k/fkYAML/pull/203) ([fktn-k](https://github.com/fktn-k))
- \#195 Implement insertion/extraction operators for basic\_node template class [\#201](https://github.com/fktn-k/fkYAML/pull/201) ([fktn-k](https://github.com/fktn-k))


#### :bug: Bug Fixes

- \#211 Fix indentation handling [\#213](https://github.com/fktn-k/fkYAML/pull/213) ([fktn-k](https://github.com/fktn-k))
- \#211 fixed bug in parsing single quoted strings [\#212](https://github.com/fktn-k/fkYAML/pull/212) ([fktn-k](https://github.com/fktn-k))
- \#205 Detect duplicate keys as an error [\#206](https://github.com/fktn-k/fkYAML/pull/206) ([fktn-k](https://github.com/fktn-k))
- \#200 Allow a space in unquoted strings [\#202](https://github.com/fktn-k/fkYAML/pull/202) ([fktn-k](https://github.com/fktn-k))

#### :memo: Documentation

- Unified doxygen comment style [\#204](https://github.com/fktn-k/fkYAML/pull/204) ([fktn-k](https://github.com/fktn-k))

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.2.0...v0.2.1

---

## **fkYAML version 0.2.0**

!!! abstract "Release Packages"

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.2.0/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.2.0/fkYAML.zip)

### Summary

This release contains a breaking change in the default type for YAML mapping node: it will no longer preserve insertion order of internal key/value pairs by default.  
To preserve the insertion order as it used to, use `fkyaml::ordered_map` as `basic_node`'s mapping node type.  
Furthermore, factory methods for scalar values of `basic_node` type have been removed and unified into the `basic_node` constructors.  
Along with the above changes, internal operations in the fkYAML have also been improved regarding conversions between YAML nodes and values of native data types.  

!!! warning CAVEAT

    The fkYAML library is an alpha quality software, at most.  
    It would take a while until this library gets stabled version, since implementation and maintenance has been/will be done by a single developer.  
    If you find a bug or wanted features, please open a discussion/issue to let me know.  
    Any kind of contribution is very much welcomed.  

### What's Changed

#### :boom: Breaking Changes

* \#180 clean up public apis [\#183](https://github.com/fktn-k/fkYAML/pull/183) ([fktn-k](https://github.com/fktn-k))
* \#167 Use std::map as the default mapping node type [\#174](https://github.com/fktn-k/fkYAML/pull/174) ([fktn-k](https://github.com/fktn-k))

#### :zap: Improvements

* \#187 support compatible strings as an argument in basic\_node ctors [\#188](https://github.com/fktn-k/fkYAML/pull/188) ([fktn-k](https://github.com/fktn-k))
* \#180 Generalize getter APIs for node value references [\#184](https://github.com/fktn-k/fkYAML/pull/184) ([fktn-k](https://github.com/fktn-k))
* \#105 Support std::initializer\_list in basic\_node constructors [\#173](https://github.com/fktn-k/fkYAML/pull/173) ([fktn-k](https://github.com/fktn-k))

#### :bug: Bug Fixes

* \#185 fixed missing calls for fclose\(\) in input adapter tests [\#186](https://github.com/fktn-k/fkYAML/pull/186) ([fktn-k](https://github.com/fktn-k))
* \#175 support detecting indentation in deserialization [\#176](https://github.com/fktn-k/fkYAML/pull/176) ([fktn-k](https://github.com/fktn-k))

#### :robot: CI

* Updated header template and target file for version up script [\#197](https://github.com/fktn-k/fkYAML/pull/197) ([fktn-k](https://github.com/fktn-k))
* \#177 Add test for input adapters only [\#178](https://github.com/fktn-k/fkYAML/pull/178) ([fktn-k](https://github.com/fktn-k))

#### :memo: Documentation

* \#179 Added integration with Google Analytics 4 [\#196](https://github.com/fktn-k/fkYAML/pull/196) ([fktn-k](https://github.com/fktn-k))
* \#179 Updated pages in the Home section in the documentation [\#194](https://github.com/fktn-k/fkYAML/pull/194) ([fktn-k](https://github.com/fktn-k))
* \#179 Migrate the gh-page content with MkDocs [\#193](https://github.com/fktn-k/fkYAML/pull/193) ([fktn-k](https://github.com/fktn-k))
* \#179 Created tutorial pages [\#192](https://github.com/fktn-k/fkYAML/pull/192) ([fktn-k](https://github.com/fktn-k))
* \#179 Migrate API docs with MkDocs [\#189](https://github.com/fktn-k/fkYAML/pull/189) ([fktn-k](https://github.com/fktn-k))
* \#179 introduce mkdocs for documentation [\#182](https://github.com/fktn-k/fkYAML/pull/182) ([fktn-k](https://github.com/fktn-k))

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.1.3...v0.2.0

---

## **fkYAML version 0.1.3**

!!! abstract "Release Packages"

    * [fkYAML.tgz](https://github.com/fktn-k/fkYAML/releases/download/v0.1.3/fkYAML.tgz)
    * [fkYAML.zip](https://github.com/fktn-k/fkYAML/releases/download/v0.1.3/fkYAML.zip)

### Summary
This release contains improvements of the fkYAML library's internal operations with some reorganization of CMake scripts.  
In addition, the analysis result from Codacy has become shown at the top of the README.md file.  

!!! warning "CAVEAT"

    The fkYAML library is an alpha quality software, at most.  
    It would take a while until this library gets stabled version, since implementation and maintenance has been/will be done by a single developer.  
    If you find a bug or wanted features, please open a discussion/issue to let me know.  
    Any kind of contribution is very much welcomed.  

### What's Changed

#### :zap: Improvements
* \#145 Expand swap() support for basic node by @fktn-k in https://github.com/fktn-k/fkYAML/pull/163
* \#18 Resolve warnings while building library/tests by @fktn-k in https://github.com/fktn-k/fkYAML/pull/165
* \#164 Use default initial values for class member variables by @fktn-k in https://github.com/fktn-k/fkYAML/pull/168
* \#158 Added to_string() for serialization of YAML nodes. by @fktn-k in https://github.com/fktn-k/fkYAML/pull/170
* \#166 Organize CMake scripts for testing by @fktn-k in https://github.com/fktn-k/fkYAML/pull/171

#### :hammer: Tool/Service Integrations
* \#160 Added the Codacy badge to README.md by @fktn-k in https://github.com/fktn-k/fkYAML/pull/162

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.1.2...v0.1.3

---

## **fkYAML version 0.1.2**

### Summary
This release contains improvements regarding internal implementation of the fkYAML library.  
Furthermore, new unit test cases have been added and the coverage result (C1) is now 100%.  

!!! warning "CAVEAT"

    The fkYAML library is an alpha quality software, at most.  
    It would take a while until this library gets stabled version, since implementation and maintenance has been/will be done by a single developer.  
    If you find a bug or wanted features, please open a discussion/issue to let me know.  
    Any kind of contribution is very much welcomed.  

### What's Changed

#### :zap: Improvements
* \#133 refactor lexer by @fktn-k in https://github.com/fktn-k/fkYAML/pull/146

#### :bug: Bug Fixes
* Fix version_macros.hpp file path in Makefile by @fktn-k in https://github.com/fktn-k/fkYAML/pull/147

#### :robot: CI
* \#150 Add unit tests for input_handler class by @fktn-k in https://github.com/fktn-k/fkYAML/pull/151
* \#150 Excluded unreachable lines in serializer class test from coverage data by @fktn-k in https://github.com/fktn-k/fkYAML/pull/152
* \#150 Covered a missing branch in ordered_map class by @fktn-k in https://github.com/fktn-k/fkYAML/pull/153
* \#150 added/modified unit tests for lexical_analyzer test by @fktn-k in https://github.com/fktn-k/fkYAML/pull/154
* \#150 Added tests  for basic_deserializer class by @fktn-k in https://github.com/fktn-k/fkYAML/pull/155

#### :memo: Documentation
* Updated documents by @fktn-k in https://github.com/fktn-k/fkYAML/pull/156

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.1.1...v0.1.2

---

## **fkYAML version 0.1.1**

### Summary
This release contains a hotfix for the test suite to check CMake integration with FetchContent module which uses the old fkYAML APIs.  
All changes are backward-compatible. See [release notes of version 0.1.0](https://github.com/fktn-k/fkYAML/releases/tag/v0.1.0) for more information on other features.  

#### CAVEAT
The fkYAML library is a alpha quality software.
Implementation and maintenance has been/will be done by a single developer.
If you find a bug or wanted features, please open a discussion/issue to let us know. Any kind of contribution is very much welcomed.

### What's Changed

#### :bug: Bug Fixes
- use the v0.1.0 fkYAML APIs in the cmake_fetch_content_test (commit hash: https://github.com/fktn-k/fkYAML/commit/2253217c4fe4d83cfe845d77675c3a1ae412c62e) by @fktn-k 

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.1.0...v0.1.1

---

## **fkYAML version 0.1.0**

### Summary
This release contains a huge change in public APIs, including naming conventions and migration of serialization/deserialization features to fkyaml::basic_node class APIs.  
Furthermore, internal implementation of the fkYAML library has been intentionally moved to `include/fkYAML/detail`'s subdirectories so that we can provide as much backward compatibility as possible for the future releases.  

#### CAVEAT  
The fkYAML library is a alpha quality software.  
Implementation and maintenance has been/will be done by a single developer.  
If you find a bug or wanted features, please open a discussion/issue to let us know. Any kind of contribution is very much welcomed.  

### What's Changed
#### :zap: Improvements
* \#123 Change naming conventions to lower_snake_case by @fktn-k in https://github.com/fktn-k/fkYAML/pull/124
* \#123 Changed file naming convention by @fktn-k in https://github.com/fktn-k/fkYAML/pull/125
* \#127 Generalize conversions between nodes and native data by @fktn-k in https://github.com/fktn-k/fkYAML/pull/129
* \#128 moved internal impl to detail dir/namespace by @fktn-k in https://github.com/fktn-k/fkYAML/pull/131
* \#126 Generalize serialization/deserialization features by @fktn-k in https://github.com/fktn-k/fkYAML/pull/134
* \#132 Classify source files in detail directory by @fktn-k in https://github.com/fktn-k/fkYAML/pull/137
* \#135 optimize build scripts by @fktn-k in https://github.com/fktn-k/fkYAML/pull/138

#### :memo: Documentation
* \#127 Update examples in readme by @fktn-k in https://github.com/fktn-k/fkYAML/pull/141

#### :robot: CI
* \#136 Auto-generate a helper source file for the clang-tidy tool by @fktn-k in https://github.com/fktn-k/fkYAML/pull/139
* \#128 fixed filtering source files to generate full-spec coverage by @fktn-k in https://github.com/fktn-k/fkYAML/pull/140

**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.0.0...v0.1.0

---

## **fkYAML version 0.0.1**

### Summary
This release improves internal operations in the fkYAML library and adds unit tests for the existing classes.  
Furthermore, several tool/service integrations has been incorporated for better maintainability.  

### What's Changed

#### :zap: Improvements
* \#34 add unit tests for deserializer by @fktn-k in https://github.com/fktn-k/fkYAML/pull/97
* Expand usable key types by @fktn-k in https://github.com/fktn-k/fkYAML/pull/115

#### :hammer: Tool/Service Integrations
* \#101 added memory leak check with Valgrind by @fktn-k in https://github.com/fktn-k/fkYAML/pull/108
* \#116 use reuse software for file headers by @fktn-k in https://github.com/fktn-k/fkYAML/pull/117
* \#118 introduce cmake-format by @fktn-k in https://github.com/fktn-k/fkYAML/pull/119
* \#120 added target to use github-changelog-generator tool by @fktn-k in https://github.com/fktn-k/fkYAML/pull/121

#### :robot: CI
* \#109 Use official docker image for GCC during CI jobs by @fktn-k in https://github.com/fktn-k/fkYAML/pull/110
* \#111 Use docker images for Clang compilers during CI jobs by @fktn-k in https://github.com/fktn-k/fkYAML/pull/112

#### :memo: Documentation
* \#113 Generate API documentation only for public members by @fktn-k in https://github.com/fktn-k/fkYAML/pull/114

#### :people_holding_hands: Community
* \#21 Update issue templates by @fktn-k in https://github.com/fktn-k/fkYAML/pull/100


**Full Changelog**: https://github.com/fktn-k/fkYAML/compare/v0.0.0...v0.0.1

---

## **fkYAML v0.0.0**

This release provides basic features of the fkYAML library.  
Major known issues and limitations are as follows:  

- %TAG directives are not supported. (Will surely be implemented.)
- YAML key must be a string literal. (YAML specification of any version does not restrict YAML keys as such.)
- Backward compatibilities for YAML 1.1 or older have not yet been implemented.

The other issues and limitations will also be fixed in later versions.  
Wait for such a version, or create a PR to fix one.  
