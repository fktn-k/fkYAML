# Changelog

## [v0.2.3](https://github.com/fktn-k/fkYAML/releases/tag/v0.2.3) (2023-12-03)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.2.2...v0.2.3)

- Feature/232 user defined literal for deserialization [\#235](https://github.com/fktn-k/fkYAML/pull/235) ([fktn-k](https://github.com/fktn-k))
- \#159 Support block scalar styles [\#228](https://github.com/fktn-k/fkYAML/pull/228) ([fktn-k](https://github.com/fktn-k))
- Feature/226 add natvis file [\#227](https://github.com/fktn-k/fkYAML/pull/227) ([fktn-k](https://github.com/fktn-k))

- fixed parsing UTF-16 encoded inputs which do not end with a surrogate… [\#234](https://github.com/fktn-k/fkYAML/pull/234) ([fktn-k](https://github.com/fktn-k))

- Improved contribution guide [\#231](https://github.com/fktn-k/fkYAML/pull/231) ([fktn-k](https://github.com/fktn-k))
- Removed dependency on the cmake-format tool [\#230](https://github.com/fktn-k/fkYAML/pull/230) ([fktn-k](https://github.com/fktn-k))
- Fixed wrong URLs in the releases page of the documentation [\#225](https://github.com/fktn-k/fkYAML/pull/225) ([fktn-k](https://github.com/fktn-k))

## [v0.2.2](https://github.com/fktn-k/fkYAML/releases/tag/v0.2.2) (2023-11-27)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.2.1...v0.2.2)

- \#209 Support auto-detection of Unicode encoding types including BOMs [\#223](https://github.com/fktn-k/fkYAML/pull/223) ([fktn-k](https://github.com/fktn-k))
- \#214 Emit more descriptive errors [\#222](https://github.com/fktn-k/fkYAML/pull/222) ([fktn-k](https://github.com/fktn-k))
- \#209 support UTF-16 / UTF-32 for deserialization input characters [\#221](https://github.com/fktn-k/fkYAML/pull/221) ([fktn-k](https://github.com/fktn-k))

- \#218 Fixed lacking quotes & escapes during serializtion [\#220](https://github.com/fktn-k/fkYAML/pull/220) ([fktn-k](https://github.com/fktn-k))
- \#217 Fix parse errors on strings containing single quotes [\#219](https://github.com/fktn-k/fkYAML/pull/219) ([fktn-k](https://github.com/fktn-k))

## [v0.2.1](https://github.com/fktn-k/fkYAML/releases/tag/v0.2.1) (2023-11-19)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.2.0...v0.2.1)

- \#209 support escaped unicode [\#215](https://github.com/fktn-k/fkYAML/pull/215) ([fktn-k](https://github.com/fktn-k))
- \#209 Support unescaped UTF-8 input characters in deserialization [\#210](https://github.com/fktn-k/fkYAML/pull/210) ([fktn-k](https://github.com/fktn-k))
- \#207 Support markers for the end of directives/documents [\#208](https://github.com/fktn-k/fkYAML/pull/208) ([fktn-k](https://github.com/fktn-k))
- \#190 Support specialization of deserialization for a vector of user-defined type objects [\#203](https://github.com/fktn-k/fkYAML/pull/203) ([fktn-k](https://github.com/fktn-k))
- \#195 Implement insertion/extraction operators for basic\_node template class [\#201](https://github.com/fktn-k/fkYAML/pull/201) ([fktn-k](https://github.com/fktn-k))

- \#211 Fix indentation handling [\#213](https://github.com/fktn-k/fkYAML/pull/213) ([fktn-k](https://github.com/fktn-k))
- \#211 fixed bug in parsing single quoted strings [\#212](https://github.com/fktn-k/fkYAML/pull/212) ([fktn-k](https://github.com/fktn-k))
- \#205 Detect duplicate keys as an error [\#206](https://github.com/fktn-k/fkYAML/pull/206) ([fktn-k](https://github.com/fktn-k))
- \#200 Allow a space in unquoted strings [\#202](https://github.com/fktn-k/fkYAML/pull/202) ([fktn-k](https://github.com/fktn-k))

- Unified doxygen comment style [\#204](https://github.com/fktn-k/fkYAML/pull/204) ([fktn-k](https://github.com/fktn-k))

## [v0.2.0](https://github.com/fktn-k/fkYAML/releases/tag/v0.2.0) (2023-11-06)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.1.3...v0.2.0)

- \#187 support compatible strings as an argument in basic\_node ctors [\#188](https://github.com/fktn-k/fkYAML/pull/188) ([fktn-k](https://github.com/fktn-k))
- \#180 Generalize getter APIs for node value references [\#184](https://github.com/fktn-k/fkYAML/pull/184) ([fktn-k](https://github.com/fktn-k))
- \#180 clean up public apis [\#183](https://github.com/fktn-k/fkYAML/pull/183) ([fktn-k](https://github.com/fktn-k))
- \#177 Add test for input adapters only [\#178](https://github.com/fktn-k/fkYAML/pull/178) ([fktn-k](https://github.com/fktn-k))
- \#167 Use std::map as the default mapping node type [\#174](https://github.com/fktn-k/fkYAML/pull/174) ([fktn-k](https://github.com/fktn-k))
- \#105 Support std::initializer\_list in basic\_node constructors [\#173](https://github.com/fktn-k/fkYAML/pull/173) ([fktn-k](https://github.com/fktn-k))

- \#185 fixed missing calls for fclose\(\) in input adapter tests [\#186](https://github.com/fktn-k/fkYAML/pull/186) ([fktn-k](https://github.com/fktn-k))
- \#175 support detecting indentation in deserialization [\#176](https://github.com/fktn-k/fkYAML/pull/176) ([fktn-k](https://github.com/fktn-k))

- Updated header template and target file for version up script [\#197](https://github.com/fktn-k/fkYAML/pull/197) ([fktn-k](https://github.com/fktn-k))
- \#179 Added integration with Google Analytics 4 [\#196](https://github.com/fktn-k/fkYAML/pull/196) ([fktn-k](https://github.com/fktn-k))
- \#179 Updated pages in the Home section in the documentation [\#194](https://github.com/fktn-k/fkYAML/pull/194) ([fktn-k](https://github.com/fktn-k))
- \#179 Migrate the gh-page content with MkDocs [\#193](https://github.com/fktn-k/fkYAML/pull/193) ([fktn-k](https://github.com/fktn-k))
- \#179 Created tutorial pages [\#192](https://github.com/fktn-k/fkYAML/pull/192) ([fktn-k](https://github.com/fktn-k))
- \#179 Migrate API docs with MkDocs [\#189](https://github.com/fktn-k/fkYAML/pull/189) ([fktn-k](https://github.com/fktn-k))
- \#179 introduce mkdocs for documentation [\#182](https://github.com/fktn-k/fkYAML/pull/182) ([fktn-k](https://github.com/fktn-k))

## [v0.1.3](https://github.com/fktn-k/fkYAML/releases/tag/v0.1.3) (2023-10-21)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.1.2...v0.1.3)

- \#166 Organize CMake scripts for testing [\#171](https://github.com/fktn-k/fkYAML/pull/171) ([fktn-k](https://github.com/fktn-k))
- \#158 Added to\_string\(\) for serialization of YAML nodes. [\#170](https://github.com/fktn-k/fkYAML/pull/170) ([fktn-k](https://github.com/fktn-k))
- \#164 Use default initial values for class member variables [\#168](https://github.com/fktn-k/fkYAML/pull/168) ([fktn-k](https://github.com/fktn-k))
- \#18 Resolve warnings while building library/tests [\#165](https://github.com/fktn-k/fkYAML/pull/165) ([fktn-k](https://github.com/fktn-k))
- \#145 Expand swap\(\) support for basic node [\#163](https://github.com/fktn-k/fkYAML/pull/163) ([fktn-k](https://github.com/fktn-k))

- \#160 Added the Codacy badge to README.md [\#162](https://github.com/fktn-k/fkYAML/pull/162) ([fktn-k](https://github.com/fktn-k))

## [v0.1.2](https://github.com/fktn-k/fkYAML/releases/tag/v0.1.2) (2023-10-18)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.1.1...v0.1.2)

- \#150 Added tests  for basic\_deserializer class [\#155](https://github.com/fktn-k/fkYAML/pull/155) ([fktn-k](https://github.com/fktn-k))
- \#150 added/modified unit tests for lexical\_analyzer test [\#154](https://github.com/fktn-k/fkYAML/pull/154) ([fktn-k](https://github.com/fktn-k))
- \#150 Covered a missing branch in ordered\_map class [\#153](https://github.com/fktn-k/fkYAML/pull/153) ([fktn-k](https://github.com/fktn-k))
- \#150 Excluded unreachable lines in serializer class test from coverage data [\#152](https://github.com/fktn-k/fkYAML/pull/152) ([fktn-k](https://github.com/fktn-k))
- \#150 Add unit tests for input\_handler class [\#151](https://github.com/fktn-k/fkYAML/pull/151) ([fktn-k](https://github.com/fktn-k))
- \#133 refactor lexer [\#146](https://github.com/fktn-k/fkYAML/pull/146) ([fktn-k](https://github.com/fktn-k))

- Fix version\_macros.hpp file path in Makefile [\#147](https://github.com/fktn-k/fkYAML/pull/147) ([fktn-k](https://github.com/fktn-k))

- Updated documents [\#156](https://github.com/fktn-k/fkYAML/pull/156) ([fktn-k](https://github.com/fktn-k))

## [v0.1.1](https://github.com/fktn-k/fkYAML/releases/tag/v0.1.1) (2023-10-15)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.1.0...v0.1.1)

## [v0.1.0](https://github.com/fktn-k/fkYAML/releases/tag/v0.1.0) (2023-10-15)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.0.1...v0.1.0)

- \#136 Auto-generate a helper source file for the clang-tidy tool [\#139](https://github.com/fktn-k/fkYAML/pull/139) ([fktn-k](https://github.com/fktn-k))
- \#135 optimize build scripts [\#138](https://github.com/fktn-k/fkYAML/pull/138) ([fktn-k](https://github.com/fktn-k))
- \#132 Classify source files in detail directory [\#137](https://github.com/fktn-k/fkYAML/pull/137) ([fktn-k](https://github.com/fktn-k))
- \#126 Generalize serialization/deserialization features [\#134](https://github.com/fktn-k/fkYAML/pull/134) ([fktn-k](https://github.com/fktn-k))
- \#127 Generalize conversions between nodes and native data [\#129](https://github.com/fktn-k/fkYAML/pull/129) ([fktn-k](https://github.com/fktn-k))

- \#128 fixed filtering source files to generate full-spec coverage [\#140](https://github.com/fktn-k/fkYAML/pull/140) ([fktn-k](https://github.com/fktn-k))

- \#127 Update examples in readme [\#141](https://github.com/fktn-k/fkYAML/pull/141) ([fktn-k](https://github.com/fktn-k))
- \#128 moved internal impl to detail dir/namespace [\#131](https://github.com/fktn-k/fkYAML/pull/131) ([fktn-k](https://github.com/fktn-k))
- \#123 Changed file naming convention [\#125](https://github.com/fktn-k/fkYAML/pull/125) ([fktn-k](https://github.com/fktn-k))
- \#123 Change naming conventions to lower\_snake\_case [\#124](https://github.com/fktn-k/fkYAML/pull/124) ([fktn-k](https://github.com/fktn-k))

## [v0.0.1](https://github.com/fktn-k/fkYAML/releases/tag/v0.0.1) (2023-10-06)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.0.0...v0.0.1)

- \#118 introduce cmake-format [\#119](https://github.com/fktn-k/fkYAML/pull/119) ([fktn-k](https://github.com/fktn-k))
- Expand usable key types [\#115](https://github.com/fktn-k/fkYAML/pull/115) ([fktn-k](https://github.com/fktn-k))
- \#113 Generate API documentation only for public members [\#114](https://github.com/fktn-k/fkYAML/pull/114) ([fktn-k](https://github.com/fktn-k))
- \#111 Use docker images for Clang compilers during CI jobs [\#112](https://github.com/fktn-k/fkYAML/pull/112) ([fktn-k](https://github.com/fktn-k))
- \#109 Use official docker image for GCC during CI jobs [\#110](https://github.com/fktn-k/fkYAML/pull/110) ([fktn-k](https://github.com/fktn-k))

- \#120 added target to use github-changelog-generator tool [\#121](https://github.com/fktn-k/fkYAML/pull/121) ([fktn-k](https://github.com/fktn-k))
- \#116 use reuse software for file headers [\#117](https://github.com/fktn-k/fkYAML/pull/117) ([fktn-k](https://github.com/fktn-k))
- \#101 added memory leak check with Valgrind [\#108](https://github.com/fktn-k/fkYAML/pull/108) ([fktn-k](https://github.com/fktn-k))
- \#21 Update issue templates [\#100](https://github.com/fktn-k/fkYAML/pull/100) ([fktn-k](https://github.com/fktn-k))
- \#34 add unit tests for deserializer [\#97](https://github.com/fktn-k/fkYAML/pull/97) ([fktn-k](https://github.com/fktn-k))

## [v0.0.0](https://github.com/fktn-k/fkYAML/releases/tag/v0.0.0) (2023-09-23)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/95b9ee09cc0fe0a609198f2754512c359c31dd6e...v0.0.0)

- \#88 add more documents [\#89](https://github.com/fktn-k/fkYAML/pull/89) ([fktn-k](https://github.com/fktn-k))
- \#83 Add compiler support on Windows & macOS [\#87](https://github.com/fktn-k/fkYAML/pull/87) ([fktn-k](https://github.com/fktn-k))
- \#85 change integer scalar data type to int64\_t only [\#86](https://github.com/fktn-k/fkYAML/pull/86) ([fktn-k](https://github.com/fktn-k))
- \#81 Disable clang-format & clang-tidy by default [\#84](https://github.com/fktn-k/fkYAML/pull/84) ([fktn-k](https://github.com/fktn-k))
- \#17 add descriptions in readme [\#82](https://github.com/fktn-k/fkYAML/pull/82) ([fktn-k](https://github.com/fktn-k))
- \#79 support include-what-you-use [\#80](https://github.com/fktn-k/fkYAML/pull/80) ([fktn-k](https://github.com/fktn-k))
- \#77 Run sanitizer only on self-hosted runners [\#78](https://github.com/fktn-k/fkYAML/pull/78) ([fktn-k](https://github.com/fktn-k))
- \#74 Fixed keeping insertion orders of mapping node items [\#76](https://github.com/fktn-k/fkYAML/pull/76) ([fktn-k](https://github.com/fktn-k))
- \#5 implemented yaml node serializer [\#75](https://github.com/fktn-k/fkYAML/pull/75) ([fktn-k](https://github.com/fktn-k))
- \#72 Added inline namespace for library versioning [\#73](https://github.com/fktn-k/fkYAML/pull/73) ([fktn-k](https://github.com/fktn-k))
- \#70 Fixed Git tag for FetchContent test [\#71](https://github.com/fktn-k/fkYAML/pull/71) ([fktn-k](https://github.com/fktn-k))
- \#62 Added build integration tests [\#69](https://github.com/fktn-k/fkYAML/pull/69) ([fktn-k](https://github.com/fktn-k))
- \#66 Added tests for various C++ versions [\#68](https://github.com/fktn-k/fkYAML/pull/68) ([fktn-k](https://github.com/fktn-k))
- \#64 add self-hosted runner for GitHub Actions [\#65](https://github.com/fktn-k/fkYAML/pull/65) ([fktn-k](https://github.com/fktn-k))
- \#61 Support YAML version directives [\#63](https://github.com/fktn-k/fkYAML/pull/63) ([fktn-k](https://github.com/fktn-k))
- \#14 Support clang-tidy on CI [\#60](https://github.com/fktn-k/fkYAML/pull/60) ([fktn-k](https://github.com/fktn-k))
- \#33 Added unit test cases for LexicalAnalyzer class [\#59](https://github.com/fktn-k/fkYAML/pull/59) ([fktn-k](https://github.com/fktn-k))
- \#57 Added static/runtime assertion checks [\#58](https://github.com/fktn-k/fkYAML/pull/58) ([fktn-k](https://github.com/fktn-k))
- \#35 Add feature of deserializing anchors & aliases [\#56](https://github.com/fktn-k/fkYAML/pull/56) ([fktn-k](https://github.com/fktn-k))
- \#54 add unit tests for Exception class [\#55](https://github.com/fktn-k/fkYAML/pull/55) ([fktn-k](https://github.com/fktn-k))
- Feature/42 add key check api to basicnode [\#53](https://github.com/fktn-k/fkYAML/pull/53) ([fktn-k](https://github.com/fktn-k))
- \#20 add unit test for iterator [\#52](https://github.com/fktn-k/fkYAML/pull/52) ([fktn-k](https://github.com/fktn-k))
- \#50 add lacked test cases for Node class [\#51](https://github.com/fktn-k/fkYAML/pull/51) ([fktn-k](https://github.com/fktn-k))
- \#45 add code coverage job [\#49](https://github.com/fktn-k/fkYAML/pull/49) ([fktn-k](https://github.com/fktn-k))
- \#17 add badges to readme [\#47](https://github.com/fktn-k/fkYAML/pull/47) ([fktn-k](https://github.com/fktn-k))
- add CodeQL analysis to CI [\#46](https://github.com/fktn-k/fkYAML/pull/46) ([fktn-k](https://github.com/fktn-k))
- \#41 introduce catch2 v2 lib to unit test app [\#44](https://github.com/fktn-k/fkYAML/pull/44) ([fktn-k](https://github.com/fktn-k))
- \#35 support anchors & aliases in Node class [\#43](https://github.com/fktn-k/fkYAML/pull/43) ([fktn-k](https://github.com/fktn-k))
- \#39 changed newline code from CRLF to LF [\#40](https://github.com/fktn-k/fkYAML/pull/40) ([fktn-k](https://github.com/fktn-k))
- \#28 add sanitizers to unit test app [\#38](https://github.com/fktn-k/fkYAML/pull/38) ([fktn-k](https://github.com/fktn-k))
- \#36 support custom node value data types [\#37](https://github.com/fktn-k/fkYAML/pull/37) ([fktn-k](https://github.com/fktn-k))
- \#4 add block mapping deserialization [\#31](https://github.com/fktn-k/fkYAML/pull/31) ([fktn-k](https://github.com/fktn-k))
- \#19 add unit test cases for node [\#30](https://github.com/fktn-k/fkYAML/pull/30) ([fktn-k](https://github.com/fktn-k))
- \#4 supported block sequence deserialization [\#29](https://github.com/fktn-k/fkYAML/pull/29) ([fktn-k](https://github.com/fktn-k))
- regulated package workflow trigger events & deleted unnecessary comments [\#27](https://github.com/fktn-k/fkYAML/pull/27) ([fktn-k](https://github.com/fktn-k))
- \#4 implement yaml deserialization [\#26](https://github.com/fktn-k/fkYAML/pull/26) ([fktn-k](https://github.com/fktn-k))
- \#10 support doxygen [\#12](https://github.com/fktn-k/fkYAML/pull/12) ([fktn-k](https://github.com/fktn-k))
- \#8 support clang format & clang tidy [\#11](https://github.com/fktn-k/fkYAML/pull/11) ([fktn-k](https://github.com/fktn-k))
- \#3 implement yaml node [\#9](https://github.com/fktn-k/fkYAML/pull/9) ([fktn-k](https://github.com/fktn-k))
- \#2 configure CI/CD [\#7](https://github.com/fktn-k/fkYAML/pull/7) ([fktn-k](https://github.com/fktn-k))
- \#1 create blank project [\#6](https://github.com/fktn-k/fkYAML/pull/6) ([fktn-k](https://github.com/fktn-k))



\* *This Changelog was automatically generated by [github_changelog_generator](https://github.com/github-changelog-generator/github-changelog-generator)*
