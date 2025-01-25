# Changelog

## [v0.4.2](https://github.com/fktn-k/fkYAML/releases/tag/v0.4.2) (2025-01-25)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.4.1...v0.4.2)

- Fix document marker detections [\#472](https://github.com/fktn-k/fkYAML/pull/472) ([fktn-k](https://github.com/fktn-k))
- Fix error in parsing empty inputs [\#471](https://github.com/fktn-k/fkYAML/pull/471) ([fktn-k](https://github.com/fktn-k))
- Fix comment prefix check [\#469](https://github.com/fktn-k/fkYAML/pull/469) ([fktn-k](https://github.com/fktn-k))
- Fix conversions from empty tuples [\#467](https://github.com/fktn-k/fkYAML/pull/467) ([fktn-k](https://github.com/fktn-k))

- Fix example builds with incorrectly indented document markers [\#473](https://github.com/fktn-k/fkYAML/pull/473) ([fktn-k](https://github.com/fktn-k))
- Refactor UTF encode validation & line break normalization [\#470](https://github.com/fktn-k/fkYAML/pull/470) ([fktn-k](https://github.com/fktn-k))
- Allow ':' adjacent to a flow mapping value right after a JSON-like key [\#468](https://github.com/fktn-k/fkYAML/pull/468) ([fktn-k](https://github.com/fktn-k))
- Disable the fail-fast option for GitHub Actions workflow jobs [\#466](https://github.com/fktn-k/fkYAML/pull/466) ([fktn-k](https://github.com/fktn-k))
- Add to\_node\(\) implementations for STL container types [\#465](https://github.com/fktn-k/fkYAML/pull/465) ([fktn-k](https://github.com/fktn-k))
- Fix example builds and typos in docs [\#464](https://github.com/fktn-k/fkYAML/pull/464) ([fktn-k](https://github.com/fktn-k))
- Improve handling node properties in deserialization [\#463](https://github.com/fktn-k/fkYAML/pull/463) ([fktn-k](https://github.com/fktn-k))

## [v0.4.1](https://github.com/fktn-k/fkYAML/releases/tag/v0.4.1) (2025-01-11)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.4.0...v0.4.1)

- Fix: made sure that empty documents just serialize to \[\] or {} [\#460](https://github.com/fktn-k/fkYAML/pull/460) ([johan-gson](https://github.com/johan-gson))
- fix: Fixed a bug where serialize didn't write out empty vectors and mappings [\#456](https://github.com/fktn-k/fkYAML/pull/456) ([johan-gson](https://github.com/johan-gson))
- Fix bad indentation detection for block collection entries [\#451](https://github.com/fktn-k/fkYAML/pull/451) ([fktn-k](https://github.com/fktn-k))
- Emit an error if ":" is missing after a mapping key [\#450](https://github.com/fktn-k/fkYAML/pull/450) ([fktn-k](https://github.com/fktn-k))

- Add test cases for serializing root empty collections [\#461](https://github.com/fktn-k/fkYAML/pull/461) ([fktn-k](https://github.com/fktn-k))
- Mapping key/value access in range based for loops [\#458](https://github.com/fktn-k/fkYAML/pull/458) ([fktn-k](https://github.com/fktn-k))
- Add test cases for serializing empty collection nodes [\#457](https://github.com/fktn-k/fkYAML/pull/457) ([fktn-k](https://github.com/fktn-k))
- Fix invalid paths for documentation & other tools [\#453](https://github.com/fktn-k/fkYAML/pull/453) ([fktn-k](https://github.com/fktn-k))
- Update copyright year and directory structure [\#452](https://github.com/fktn-k/fkYAML/pull/452) ([fktn-k](https://github.com/fktn-k))
- Optimize lexer implementation [\#448](https://github.com/fktn-k/fkYAML/pull/448) ([fktn-k](https://github.com/fktn-k))
- Update workflow jobs for release artifacts [\#447](https://github.com/fktn-k/fkYAML/pull/447) ([fktn-k](https://github.com/fktn-k))
- Revise the contents of API References [\#445](https://github.com/fktn-k/fkYAML/pull/445) ([fktn-k](https://github.com/fktn-k))
- support std::forward\_list in from\_node [\#444](https://github.com/fktn-k/fkYAML/pull/444) ([fktn-k](https://github.com/fktn-k))
- Add get\_value\_inplace API to basic\_node [\#443](https://github.com/fktn-k/fkYAML/pull/443) ([fktn-k](https://github.com/fktn-k))
- Resolve CMake deprecation warning [\#442](https://github.com/fktn-k/fkYAML/pull/442) ([fktn-k](https://github.com/fktn-k))

## [v0.4.0](https://github.com/fktn-k/fkYAML/releases/tag/v0.4.0) (2024-12-10)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.14...v0.4.0)

- Fix round-trip issue in float serialization using scientific notation [\#439](https://github.com/fktn-k/fkYAML/pull/439) ([fktn-k](https://github.com/fktn-k))
- Fix parsing input which begins with a newline & indentation [\#437](https://github.com/fktn-k/fkYAML/pull/437) ([fktn-k](https://github.com/fktn-k))
- Fixed bugs in parsing block scalars [\#435](https://github.com/fktn-k/fkYAML/pull/435) ([fktn-k](https://github.com/fktn-k))
- Emit error if an anchor is specified to an alias [\#434](https://github.com/fktn-k/fkYAML/pull/434) ([fktn-k](https://github.com/fktn-k))
- Stop throwing parse\_error on string-to-int/float conversion failures if not forced with tag [\#431](https://github.com/fktn-k/fkYAML/pull/431) ([fktn-k](https://github.com/fktn-k))
- Resolve the C4800 warning when compiled with MSVC [\#430](https://github.com/fktn-k/fkYAML/pull/430) ([fktn-k](https://github.com/fktn-k))

- Support reverse iterations over sequence/mapping nodes [\#440](https://github.com/fktn-k/fkYAML/pull/440) ([fktn-k](https://github.com/fktn-k))
- Make node iterators compatible with different value type const-ness [\#438](https://github.com/fktn-k/fkYAML/pull/438) ([fktn-k](https://github.com/fktn-k))
- Add more GCC & Clang versions to use in GitHub Actions workflows [\#436](https://github.com/fktn-k/fkYAML/pull/436) ([fktn-k](https://github.com/fktn-k))
- Update GitHub Actions workflow jobs using macOS related runner images [\#433](https://github.com/fktn-k/fkYAML/pull/433) ([fktn-k](https://github.com/fktn-k))
- Support parsing multiline plain scalars [\#432](https://github.com/fktn-k/fkYAML/pull/432) ([fktn-k](https://github.com/fktn-k))

## [v0.3.14](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.14) (2024-11-16)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.13...v0.3.14)

- Disable -Wdeprecated-literal-operator warnings [\#417](https://github.com/fktn-k/fkYAML/pull/417) ([fktn-k](https://github.com/fktn-k))
- Fix wrong scalar value type detection from floating point value token [\#414](https://github.com/fktn-k/fkYAML/pull/414) ([fktn-k](https://github.com/fktn-k))

- Exclude detail::unreachable\(\) from coverage target [\#426](https://github.com/fktn-k/fkYAML/pull/426) ([fktn-k](https://github.com/fktn-k))
- Use std::unreachable\(\) or similar compiler specific extensions for dead but necessary code [\#425](https://github.com/fktn-k/fkYAML/pull/425) ([fktn-k](https://github.com/fktn-k))
- Clarify type restrictions of get\_value\(\) & get\_value\_ref\(\) APIs [\#424](https://github.com/fktn-k/fkYAML/pull/424) ([fktn-k](https://github.com/fktn-k))
- Fix duplicate include error if clang-tidy helper file already exists [\#423](https://github.com/fktn-k/fkYAML/pull/423) ([fktn-k](https://github.com/fktn-k))
- Add forward declaration header [\#422](https://github.com/fktn-k/fkYAML/pull/422) ([fktn-k](https://github.com/fktn-k))
- Support more STL types in from\_node [\#421](https://github.com/fktn-k/fkYAML/pull/421) ([fktn-k](https://github.com/fktn-k))
- Numeric scalar conversions inside basic\_node::get\_value API [\#419](https://github.com/fktn-k/fkYAML/pull/419) ([fktn-k](https://github.com/fktn-k))
- Fix somehow shadowed clang-tidy warnings [\#418](https://github.com/fktn-k/fkYAML/pull/418) ([fktn-k](https://github.com/fktn-k))
- GA workflow jobs with more AppleClang versions on macOS [\#416](https://github.com/fktn-k/fkYAML/pull/416) ([fktn-k](https://github.com/fktn-k))
- Update GA workflow jobs for ubuntu24.04 [\#415](https://github.com/fktn-k/fkYAML/pull/415) ([fktn-k](https://github.com/fktn-k))

## [v0.3.13](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.13) (2024-10-14)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.12...v0.3.13)

- Fix wrong parse result from single scalar document [\#411](https://github.com/fktn-k/fkYAML/pull/411) ([fktn-k](https://github.com/fktn-k))
- Fix infinite loops after parsing final empty block scalar [\#410](https://github.com/fktn-k/fkYAML/pull/410) ([fktn-k](https://github.com/fktn-k))
- Fix float scalar serialization when a float is actually an integer [\#407](https://github.com/fktn-k/fkYAML/pull/407) ([fktn-k](https://github.com/fktn-k))
- Fix URI validation for tag shorthands [\#403](https://github.com/fktn-k/fkYAML/pull/403) ([fktn-k](https://github.com/fktn-k))
- Updated natvis file [\#402](https://github.com/fktn-k/fkYAML/pull/402) ([fktn-k](https://github.com/fktn-k))
- Fix compile warnings/errors when benchmarker app is compiled with msvc [\#401](https://github.com/fktn-k/fkYAML/pull/401) ([fktn-k](https://github.com/fktn-k))
- Accept % as first scalar character [\#399](https://github.com/fktn-k/fkYAML/pull/399) ([fktn-k](https://github.com/fktn-k))

- Optimized scalar parsing [\#409](https://github.com/fktn-k/fkYAML/pull/409) ([fktn-k](https://github.com/fktn-k))
- Fix some typos in strings and comments [\#408](https://github.com/fktn-k/fkYAML/pull/408) ([sndth](https://github.com/sndth))
- Remove header lines for Doxygen [\#398](https://github.com/fktn-k/fkYAML/pull/398) ([fktn-k](https://github.com/fktn-k))
- Refine benchmarking [\#397](https://github.com/fktn-k/fkYAML/pull/397) ([fktn-k](https://github.com/fktn-k))

## [v0.3.12](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.12) (2024-09-21)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.11...v0.3.12)

- Parse +.inf as the positive float infinity [\#393](https://github.com/fktn-k/fkYAML/pull/393) ([fktn-k](https://github.com/fktn-k))
- Use \_MSVC\_LANG macro when compiled with MSVC for C++ standard detection [\#392](https://github.com/fktn-k/fkYAML/pull/392) ([fktn-k](https://github.com/fktn-k))
- Fix detecting invalid contents of block scalar headers [\#387](https://github.com/fktn-k/fkYAML/pull/387) ([fktn-k](https://github.com/fktn-k))

- Use likely/unlikely if available [\#395](https://github.com/fktn-k/fkYAML/pull/395) ([fktn-k](https://github.com/fktn-k))
- Reduce string copies in parse [\#394](https://github.com/fktn-k/fkYAML/pull/394) ([fktn-k](https://github.com/fktn-k))
- Improve conversion from scalars to native types [\#391](https://github.com/fktn-k/fkYAML/pull/391) ([fktn-k](https://github.com/fktn-k))
- Refactor lexical analysis [\#390](https://github.com/fktn-k/fkYAML/pull/390) ([fktn-k](https://github.com/fktn-k))
- Refactor node attributes management [\#389](https://github.com/fktn-k/fkYAML/pull/389) ([fktn-k](https://github.com/fktn-k))
- Add node\_type/yaml\_version\_type enum class APIs [\#388](https://github.com/fktn-k/fkYAML/pull/388) ([fktn-k](https://github.com/fktn-k))
- Improve UTF encoding detection [\#386](https://github.com/fktn-k/fkYAML/pull/386) ([fktn-k](https://github.com/fktn-k))

## [v0.3.11](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.11) (2024-08-24)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.10...v0.3.11)

- Fix error on parsing a floating point value which ends with a dot [\#382](https://github.com/fktn-k/fkYAML/pull/382) ([fktn-k](https://github.com/fktn-k))
- Throw exception when loading a nonexistent stream [\#379](https://github.com/fktn-k/fkYAML/pull/379) ([sndth](https://github.com/sndth))
- Fix wrong parse result from a block sequence as a nested block mapping value [\#377](https://github.com/fktn-k/fkYAML/pull/377) ([fktn-k](https://github.com/fktn-k))
- \#375 Fix newline code normalization in UTF-8 encoded file input [\#376](https://github.com/fktn-k/fkYAML/pull/376) ([fktn-k](https://github.com/fktn-k))
- Allow the document end marker omitted [\#374](https://github.com/fktn-k/fkYAML/pull/374) ([fktn-k](https://github.com/fktn-k))

- Fix conditional branching for pull\_request\_target events [\#384](https://github.com/fktn-k/fkYAML/pull/384) ([fktn-k](https://github.com/fktn-k))
- Fix workflow errors on PRs from fork repos [\#383](https://github.com/fktn-k/fkYAML/pull/383) ([fktn-k](https://github.com/fktn-k))

## [v0.3.10](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.10) (2024-08-18)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.9...v0.3.10)

- \#366 Fix node-to-float conversion error if node's value is \<= 0 [\#371](https://github.com/fktn-k/fkYAML/pull/371) ([fktn-k](https://github.com/fktn-k))
- Apply node properties for the root node [\#370](https://github.com/fktn-k/fkYAML/pull/370) ([fktn-k](https://github.com/fktn-k))
- \#368 Fix line advancement after node props [\#369](https://github.com/fktn-k/fkYAML/pull/369) ([fktn-k](https://github.com/fktn-k))
- Allow trailing comma in flow mapping/sequence [\#365](https://github.com/fktn-k/fkYAML/pull/365) ([fktn-k](https://github.com/fktn-k))
- Fix bug in serializing alias keys [\#364](https://github.com/fktn-k/fkYAML/pull/364) ([fktn-k](https://github.com/fktn-k))
- Detect missing the end of directives markers \(---\) [\#361](https://github.com/fktn-k/fkYAML/pull/361) ([fktn-k](https://github.com/fktn-k))

- update configs for reuse v4 [\#372](https://github.com/fktn-k/fkYAML/pull/372) ([fktn-k](https://github.com/fktn-k))
- Remove CI jobs running with macOS 11 [\#367](https://github.com/fktn-k/fkYAML/pull/367) ([fktn-k](https://github.com/fktn-k))
- Support serializing multiple YAML docs [\#363](https://github.com/fktn-k/fkYAML/pull/363) ([fktn-k](https://github.com/fktn-k))
- Support parssing multiple YAML documents [\#362](https://github.com/fktn-k/fkYAML/pull/362) ([fktn-k](https://github.com/fktn-k))
- Support Intel icpx compiler [\#360](https://github.com/fktn-k/fkYAML/pull/360) ([fktn-k](https://github.com/fktn-k))

## [v0.3.9](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.9) (2024-06-12)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.8...v0.3.9)

- Fix compile errors on Windows [\#358](https://github.com/fktn-k/fkYAML/pull/358) ([fktn-k](https://github.com/fktn-k))
- Fix parse error on smal negative integer keys [\#355](https://github.com/fktn-k/fkYAML/pull/355) ([fktn-k](https://github.com/fktn-k))
- fixed parse error on explicit mappings as a block sequence entry [\#353](https://github.com/fktn-k/fkYAML/pull/353) ([fktn-k](https://github.com/fktn-k))

- Refactor for optimization [\#356](https://github.com/fktn-k/fkYAML/pull/356) ([fktn-k](https://github.com/fktn-k))
- Add benchmarking tool [\#354](https://github.com/fktn-k/fkYAML/pull/354) ([fktn-k](https://github.com/fktn-k))

## [v0.3.8](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.8) (2024-06-01)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.7...v0.3.8)

- Fix parse error on empty flow containers [\#351](https://github.com/fktn-k/fkYAML/pull/351) ([fktn-k](https://github.com/fktn-k))
- Detect errors on missing flow value separators [\#350](https://github.com/fktn-k/fkYAML/pull/350) ([fktn-k](https://github.com/fktn-k))
- Fix wrong parse result from mappings containing the non-first block sequence [\#348](https://github.com/fktn-k/fkYAML/pull/348) ([fktn-k](https://github.com/fktn-k))
- Fix parse error on flow container keys [\#346](https://github.com/fktn-k/fkYAML/pull/346) ([fktn-k](https://github.com/fktn-k))
- Fix parse error on flow containers containing child flow containers [\#345](https://github.com/fktn-k/fkYAML/pull/345) ([fktn-k](https://github.com/fktn-k))

- added gcc14 & clang18 to the compiler version list for testing [\#349](https://github.com/fktn-k/fkYAML/pull/349) ([fktn-k](https://github.com/fktn-k))
- support flow line folding for single/double quoted scalars [\#344](https://github.com/fktn-k/fkYAML/pull/344) ([fktn-k](https://github.com/fktn-k))
- Normalize newline codes in the input buffer [\#343](https://github.com/fktn-k/fkYAML/pull/343) ([fktn-k](https://github.com/fktn-k))

## [v0.3.7](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.7) (2024-05-16)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.6...v0.3.7)

- Fixed error on node properties for child block sequences [\#338](https://github.com/fktn-k/fkYAML/pull/338) ([fktn-k](https://github.com/fktn-k))
- Escape backslash\(\\) + 0x09 to a horizontal tab \(\t\) [\#336](https://github.com/fktn-k/fkYAML/pull/336) ([fktn-k](https://github.com/fktn-k))
- Fix false error on anchor names containing colons \(:\) [\#335](https://github.com/fktn-k/fkYAML/pull/335) ([fktn-k](https://github.com/fktn-k))

- Run and apply the result of clang-format & amalagamation in GA workflows [\#341](https://github.com/fktn-k/fkYAML/pull/341) ([fktn-k](https://github.com/fktn-k))
- Improve anchor alias node management [\#340](https://github.com/fktn-k/fkYAML/pull/340) ([fktn-k](https://github.com/fktn-k))
- Fix the C6262 warning on Windows [\#339](https://github.com/fktn-k/fkYAML/pull/339) ([fktn-k](https://github.com/fktn-k))
- Separate YAML escaping/unescaping functionalities [\#337](https://github.com/fktn-k/fkYAML/pull/337) ([fktn-k](https://github.com/fktn-k))

## [v0.3.6](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.6) (2024-05-02)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.5...v0.3.6)

- Fixed parse error on block sequences with child block mappings split by a newline code [\#333](https://github.com/fktn-k/fkYAML/pull/333) ([fktn-k](https://github.com/fktn-k))
- Fix parse error on block sequences with child flow style container nodes [\#332](https://github.com/fktn-k/fkYAML/pull/332) ([fktn-k](https://github.com/fktn-k))
- Fixed parse error on root block sequence with child block sequences [\#330](https://github.com/fktn-k/fkYAML/pull/330) ([fktn-k](https://github.com/fktn-k))
- Allow backslashes in plain/single-quoted scalars [\#329](https://github.com/fktn-k/fkYAML/pull/329) ([fktn-k](https://github.com/fktn-k))
- Fixed parser crashes on a sequence right after the directives end marker [\#327](https://github.com/fktn-k/fkYAML/pull/327) ([fktn-k](https://github.com/fktn-k))

- Refactor deserialization process [\#331](https://github.com/fktn-k/fkYAML/pull/331) ([fktn-k](https://github.com/fktn-k))
- Fixed no such file/directory error when running Bash scripts on Windows [\#328](https://github.com/fktn-k/fkYAML/pull/328) ([fktn-k](https://github.com/fktn-k))

## [v0.3.5](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.5) (2024-04-27)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.4...v0.3.5)

- Fix parse error on the first node which isn't a block mapping [\#322](https://github.com/fktn-k/fkYAML/pull/322) ([fktn-k](https://github.com/fktn-k))

- Add Bash & Batch scripts to run clang-format & amalgamation [\#325](https://github.com/fktn-k/fkYAML/pull/325) ([fktn-k](https://github.com/fktn-k))
- Force LF newline codes in the amalgamated file [\#324](https://github.com/fktn-k/fkYAML/pull/324) ([fktn-k](https://github.com/fktn-k))
- Add workflows items to test Win32 builds [\#323](https://github.com/fktn-k/fkYAML/pull/323) ([fktn-k](https://github.com/fktn-k))
- Set the unit test app as the Visual Studio startup project [\#321](https://github.com/fktn-k/fkYAML/pull/321) ([fktn-k](https://github.com/fktn-k))
- Reformat source files [\#320](https://github.com/fktn-k/fkYAML/pull/320) ([fktn-k](https://github.com/fktn-k))
- Serialize container keys [\#319](https://github.com/fktn-k/fkYAML/pull/319) ([fktn-k](https://github.com/fktn-k))
- Improved UTF encoding processing [\#318](https://github.com/fktn-k/fkYAML/pull/318) ([fktn-k](https://github.com/fktn-k))
- Query the count of UTF-8 character bytes [\#317](https://github.com/fktn-k/fkYAML/pull/317) ([fktn-k](https://github.com/fktn-k))
- Use Clang-Format python distribution [\#316](https://github.com/fktn-k/fkYAML/pull/316) ([fktn-k](https://github.com/fktn-k))

## [v0.3.4](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.4) (2024-04-20)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.3...v0.3.4)

- Fix Valgrind error in the test suite [\#307](https://github.com/fktn-k/fkYAML/pull/307) ([fktn-k](https://github.com/fktn-k))

- \#181 Serialize container tags [\#314](https://github.com/fktn-k/fkYAML/pull/314) ([fktn-k](https://github.com/fktn-k))
- \#294 Support std::basic\_string\_view [\#313](https://github.com/fktn-k/fkYAML/pull/313) ([fktn-k](https://github.com/fktn-k))
- updated dependencies for mkdocs to the latest versions [\#312](https://github.com/fktn-k/fkYAML/pull/312) ([fktn-k](https://github.com/fktn-k))
- Organized the contents of the test suite [\#311](https://github.com/fktn-k/fkYAML/pull/311) ([fktn-k](https://github.com/fktn-k))
- Add workflow jobs with the macOS14 runner image [\#310](https://github.com/fktn-k/fkYAML/pull/310) ([fktn-k](https://github.com/fktn-k))
- support tags for container nodes with some cleanup in test suite [\#309](https://github.com/fktn-k/fkYAML/pull/309) ([fktn-k](https://github.com/fktn-k))
- Defer scalar conversions [\#308](https://github.com/fktn-k/fkYAML/pull/308) ([fktn-k](https://github.com/fktn-k))
- Scan inputs using iterators [\#306](https://github.com/fktn-k/fkYAML/pull/306) ([fktn-k](https://github.com/fktn-k))
- \#181 support tag for scalars [\#305](https://github.com/fktn-k/fkYAML/pull/305) ([fktn-k](https://github.com/fktn-k))

## [v0.3.3](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.3) (2024-03-31)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.2...v0.3.3)

- \#302 Fixed parse error on alias mapping keys [\#303](https://github.com/fktn-k/fkYAML/pull/303) ([fktn-k](https://github.com/fktn-k))
- \#292 Better handling for flow indicators in permitted scalar contexts [\#293](https://github.com/fktn-k/fkYAML/pull/293) ([stephenwhittle](https://github.com/stephenwhittle))
- \#288 Fixed incorrect parse results from mapping entries split across newlines [\#289](https://github.com/fktn-k/fkYAML/pull/289) ([fktn-k](https://github.com/fktn-k))

- Further improvements of input handlings [\#301](https://github.com/fktn-k/fkYAML/pull/301) ([fktn-k](https://github.com/fktn-k))
- Fixed warnings and made future warnings as errors [\#300](https://github.com/fktn-k/fkYAML/pull/300) ([fktn-k](https://github.com/fktn-k))
- \#298 Add at\(\) API to the basic\_node class [\#299](https://github.com/fktn-k/fkYAML/pull/299) ([fktn-k](https://github.com/fktn-k))
- Improve handling UTF encoded inputs [\#296](https://github.com/fktn-k/fkYAML/pull/296) ([fktn-k](https://github.com/fktn-k))
- \#297 Add a note for checking the coverage & upload the same as an artifact [\#295](https://github.com/fktn-k/fkYAML/pull/295) ([fktn-k](https://github.com/fktn-k))
- modied the way of formatting error messages for exception objects [\#291](https://github.com/fktn-k/fkYAML/pull/291) ([fktn-k](https://github.com/fktn-k))
- Resolve warnings against using Node.js 16 [\#290](https://github.com/fktn-k/fkYAML/pull/290) ([fktn-k](https://github.com/fktn-k))
- Add .editorconfig file [\#287](https://github.com/fktn-k/fkYAML/pull/287) ([fktn-k](https://github.com/fktn-k))

## [v0.3.2](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.2) (2024-03-17)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.1...v0.3.2)

- \#281 Fixed the parser crash due to comments right after a sequence block key [\#284](https://github.com/fktn-k/fkYAML/pull/284) ([fktn-k](https://github.com/fktn-k))
- \#282 Don't traverse up to the parent node immediately after parsing a flow-mapping value [\#283](https://github.com/fktn-k/fkYAML/pull/283) ([stephenwhittle](https://github.com/stephenwhittle))
- \#277 Fixed incorrect parse result from plain scalars starting with special values [\#278](https://github.com/fktn-k/fkYAML/pull/278) ([fktn-k](https://github.com/fktn-k))
- \#275 Fixed parse error on plain scalars containing flow indicators [\#276](https://github.com/fktn-k/fkYAML/pull/276) ([fktn-k](https://github.com/fktn-k))
- \#272 Fix parse error on a block sequence containing a comment within [\#273](https://github.com/fktn-k/fkYAML/pull/273) ([fktn-k](https://github.com/fktn-k))

- Clean up CMakeLists files [\#280](https://github.com/fktn-k/fkYAML/pull/280) ([fktn-k](https://github.com/fktn-k))
- Updated documents [\#279](https://github.com/fktn-k/fkYAML/pull/279) ([fktn-k](https://github.com/fktn-k))
- Add workflow jobs with the macos-13 runner image [\#274](https://github.com/fktn-k/fkYAML/pull/274) ([fktn-k](https://github.com/fktn-k))
- Updated copyright year [\#271](https://github.com/fktn-k/fkYAML/pull/271) ([fktn-k](https://github.com/fktn-k))
- Modify handling node properties [\#270](https://github.com/fktn-k/fkYAML/pull/270) ([fktn-k](https://github.com/fktn-k))
- \#237 Support char8\_t in deserialization [\#269](https://github.com/fktn-k/fkYAML/pull/269) ([fktn-k](https://github.com/fktn-k))

## [v0.3.1](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.1) (2023-12-21)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.3.0...v0.3.1)

- Avoid using unintended C++14 features [\#266](https://github.com/fktn-k/fkYAML/pull/266) ([fktn-k](https://github.com/fktn-k))
- Allow a dash\(-\) at the first character of a plain scalar [\#254](https://github.com/fktn-k/fkYAML/pull/254) ([fktn-k](https://github.com/fktn-k))
- \#242 Fix error in parsing string consisting only of numbers and multiple dots [\#251](https://github.com/fktn-k/fkYAML/pull/251) ([fktn-k](https://github.com/fktn-k))

- fixed typos which needs to be target\_link\_libraries\(\) in tutorials [\#267](https://github.com/fktn-k/fkYAML/pull/267) ([fktn-k](https://github.com/fktn-k))
- \#229 Fix the documentation build errors [\#265](https://github.com/fktn-k/fkYAML/pull/265) ([fktn-k](https://github.com/fktn-k))
- \#229 build example code and use output [\#264](https://github.com/fktn-k/fkYAML/pull/264) ([fktn-k](https://github.com/fktn-k))
- Added paths filter to regulate workflow executions [\#263](https://github.com/fktn-k/fkYAML/pull/263) ([fktn-k](https://github.com/fktn-k))
- \#255 serve single header file [\#261](https://github.com/fktn-k/fkYAML/pull/261) ([fktn-k](https://github.com/fktn-k))
- \#256 Added specialization of from\_node\(\) for std::map with compatible key/value types [\#260](https://github.com/fktn-k/fkYAML/pull/260) ([fktn-k](https://github.com/fktn-k))
- Allow double quotation marks in plain scalars [\#253](https://github.com/fktn-k/fkYAML/pull/253) ([fktn-k](https://github.com/fktn-k))

## [v0.3.0](https://github.com/fktn-k/fkYAML/releases/tag/v0.3.0) (2023-12-10)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.2.3...v0.3.0)

- Fixed missing callings of `fclose()` in the unit tests [\#246](https://github.com/fktn-k/fkYAML/pull/246) ([fktn-k](https://github.com/fktn-k))

- Update copyright description [\#249](https://github.com/fktn-k/fkYAML/pull/249) ([fktn-k](https://github.com/fktn-k))
- Add GCC compiler versions tested in GA workflows [\#248](https://github.com/fktn-k/fkYAML/pull/248) ([fktn-k](https://github.com/fktn-k))
- \#240 Support non-string-scalar node keys in basic\_node ctor with std::initializer\_list [\#247](https://github.com/fktn-k/fkYAML/pull/247) ([fktn-k](https://github.com/fktn-k))
- \#240 Support explicit block mappings & non-scalar nodes as mapping keys [\#245](https://github.com/fktn-k/fkYAML/pull/245) ([fktn-k](https://github.com/fktn-k))
- \#240 Support non-string scalar nodes as mapping keys [\#244](https://github.com/fktn-k/fkYAML/pull/244) ([fktn-k](https://github.com/fktn-k))
- \#240 implement node comparison [\#243](https://github.com/fktn-k/fkYAML/pull/243) ([fktn-k](https://github.com/fktn-k))
- Feature/238 apply correct noexcept [\#241](https://github.com/fktn-k/fkYAML/pull/241) ([fktn-k](https://github.com/fktn-k))

## [v0.2.3](https://github.com/fktn-k/fkYAML/releases/tag/v0.2.3) (2023-12-03)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.2.2...v0.2.3)

- fixed parsing UTF-16 encoded inputs which do not end with a surrogate… [\#234](https://github.com/fktn-k/fkYAML/pull/234) ([fktn-k](https://github.com/fktn-k))

- Feature/232 user defined literal for deserialization [\#235](https://github.com/fktn-k/fkYAML/pull/235) ([fktn-k](https://github.com/fktn-k))
- Improved contribution guide [\#231](https://github.com/fktn-k/fkYAML/pull/231) ([fktn-k](https://github.com/fktn-k))
- Removed dependency on the cmake-format tool [\#230](https://github.com/fktn-k/fkYAML/pull/230) ([fktn-k](https://github.com/fktn-k))
- \#159 Support block scalar styles [\#228](https://github.com/fktn-k/fkYAML/pull/228) ([fktn-k](https://github.com/fktn-k))
- Feature/226 add natvis file [\#227](https://github.com/fktn-k/fkYAML/pull/227) ([fktn-k](https://github.com/fktn-k))
- Fixed wrong URLs in the releases page of the documentation [\#225](https://github.com/fktn-k/fkYAML/pull/225) ([fktn-k](https://github.com/fktn-k))

## [v0.2.2](https://github.com/fktn-k/fkYAML/releases/tag/v0.2.2) (2023-11-27)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.2.1...v0.2.2)

- \#218 Fixed lacking quotes & escapes during serializtion [\#220](https://github.com/fktn-k/fkYAML/pull/220) ([fktn-k](https://github.com/fktn-k))
- \#217 Fix parse errors on strings containing single quotes [\#219](https://github.com/fktn-k/fkYAML/pull/219) ([fktn-k](https://github.com/fktn-k))

- \#209 Support auto-detection of Unicode encoding types including BOMs [\#223](https://github.com/fktn-k/fkYAML/pull/223) ([fktn-k](https://github.com/fktn-k))
- \#214 Emit more descriptive errors [\#222](https://github.com/fktn-k/fkYAML/pull/222) ([fktn-k](https://github.com/fktn-k))
- \#209 support UTF-16 / UTF-32 for deserialization input characters [\#221](https://github.com/fktn-k/fkYAML/pull/221) ([fktn-k](https://github.com/fktn-k))

## [v0.2.1](https://github.com/fktn-k/fkYAML/releases/tag/v0.2.1) (2023-11-19)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.2.0...v0.2.1)

- \#211 Fix indentation handling [\#213](https://github.com/fktn-k/fkYAML/pull/213) ([fktn-k](https://github.com/fktn-k))
- \#211 fixed bug in parsing single quoted strings [\#212](https://github.com/fktn-k/fkYAML/pull/212) ([fktn-k](https://github.com/fktn-k))
- \#205 Detect duplicate keys as an error [\#206](https://github.com/fktn-k/fkYAML/pull/206) ([fktn-k](https://github.com/fktn-k))
- \#200 Allow a space in unquoted strings [\#202](https://github.com/fktn-k/fkYAML/pull/202) ([fktn-k](https://github.com/fktn-k))

- \#209 support escaped unicode [\#215](https://github.com/fktn-k/fkYAML/pull/215) ([fktn-k](https://github.com/fktn-k))
- \#209 Support unescaped UTF-8 input characters in deserialization [\#210](https://github.com/fktn-k/fkYAML/pull/210) ([fktn-k](https://github.com/fktn-k))
- \#207 Support markers for the end of directives/documents [\#208](https://github.com/fktn-k/fkYAML/pull/208) ([fktn-k](https://github.com/fktn-k))
- Unified doxygen comment style [\#204](https://github.com/fktn-k/fkYAML/pull/204) ([fktn-k](https://github.com/fktn-k))
- \#190 Support specialization of deserialization for a vector of user-defined type objects [\#203](https://github.com/fktn-k/fkYAML/pull/203) ([fktn-k](https://github.com/fktn-k))
- \#195 Implement insertion/extraction operators for basic\_node template class [\#201](https://github.com/fktn-k/fkYAML/pull/201) ([fktn-k](https://github.com/fktn-k))

## [v0.2.0](https://github.com/fktn-k/fkYAML/releases/tag/v0.2.0) (2023-11-06)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.1.3...v0.2.0)

- \#185 fixed missing calls for fclose\(\) in input adapter tests [\#186](https://github.com/fktn-k/fkYAML/pull/186) ([fktn-k](https://github.com/fktn-k))
- \#175 support detecting indentation in deserialization [\#176](https://github.com/fktn-k/fkYAML/pull/176) ([fktn-k](https://github.com/fktn-k))

- Updated header template and target file for version up script [\#197](https://github.com/fktn-k/fkYAML/pull/197) ([fktn-k](https://github.com/fktn-k))
- \#179 Added integration with Google Analytics 4 [\#196](https://github.com/fktn-k/fkYAML/pull/196) ([fktn-k](https://github.com/fktn-k))
- \#179 Updated pages in the Home section in the documentation [\#194](https://github.com/fktn-k/fkYAML/pull/194) ([fktn-k](https://github.com/fktn-k))
- \#179 Migrate the gh-page content with MkDocs [\#193](https://github.com/fktn-k/fkYAML/pull/193) ([fktn-k](https://github.com/fktn-k))
- \#179 Created tutorial pages [\#192](https://github.com/fktn-k/fkYAML/pull/192) ([fktn-k](https://github.com/fktn-k))
- \#179 Migrate API docs with MkDocs [\#189](https://github.com/fktn-k/fkYAML/pull/189) ([fktn-k](https://github.com/fktn-k))
- \#187 support compatible strings as an argument in basic\_node ctors [\#188](https://github.com/fktn-k/fkYAML/pull/188) ([fktn-k](https://github.com/fktn-k))
- \#180 Generalize getter APIs for node value references [\#184](https://github.com/fktn-k/fkYAML/pull/184) ([fktn-k](https://github.com/fktn-k))
- \#180 clean up public apis [\#183](https://github.com/fktn-k/fkYAML/pull/183) ([fktn-k](https://github.com/fktn-k))
- \#179 introduce mkdocs for documentation [\#182](https://github.com/fktn-k/fkYAML/pull/182) ([fktn-k](https://github.com/fktn-k))
- \#177 Add test for input adapters only [\#178](https://github.com/fktn-k/fkYAML/pull/178) ([fktn-k](https://github.com/fktn-k))
- \#167 Use std::map as the default mapping node type [\#174](https://github.com/fktn-k/fkYAML/pull/174) ([fktn-k](https://github.com/fktn-k))
- \#105 Support std::initializer\_list in basic\_node constructors [\#173](https://github.com/fktn-k/fkYAML/pull/173) ([fktn-k](https://github.com/fktn-k))

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

- Fix version\_macros.hpp file path in Makefile [\#147](https://github.com/fktn-k/fkYAML/pull/147) ([fktn-k](https://github.com/fktn-k))

- Updated documents [\#156](https://github.com/fktn-k/fkYAML/pull/156) ([fktn-k](https://github.com/fktn-k))
- \#150 Added tests  for basic\_deserializer class [\#155](https://github.com/fktn-k/fkYAML/pull/155) ([fktn-k](https://github.com/fktn-k))
- \#150 added/modified unit tests for lexical\_analyzer test [\#154](https://github.com/fktn-k/fkYAML/pull/154) ([fktn-k](https://github.com/fktn-k))
- \#150 Covered a missing branch in ordered\_map class [\#153](https://github.com/fktn-k/fkYAML/pull/153) ([fktn-k](https://github.com/fktn-k))
- \#150 Excluded unreachable lines in serializer class test from coverage data [\#152](https://github.com/fktn-k/fkYAML/pull/152) ([fktn-k](https://github.com/fktn-k))
- \#150 Add unit tests for input\_handler class [\#151](https://github.com/fktn-k/fkYAML/pull/151) ([fktn-k](https://github.com/fktn-k))
- \#133 refactor lexer [\#146](https://github.com/fktn-k/fkYAML/pull/146) ([fktn-k](https://github.com/fktn-k))

## [v0.1.1](https://github.com/fktn-k/fkYAML/releases/tag/v0.1.1) (2023-10-15)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.1.0...v0.1.1)

## [v0.1.0](https://github.com/fktn-k/fkYAML/releases/tag/v0.1.0) (2023-10-15)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.0.1...v0.1.0)

- \#128 fixed filtering source files to generate full-spec coverage [\#140](https://github.com/fktn-k/fkYAML/pull/140) ([fktn-k](https://github.com/fktn-k))

- \#127 Update examples in readme [\#141](https://github.com/fktn-k/fkYAML/pull/141) ([fktn-k](https://github.com/fktn-k))
- \#136 Auto-generate a helper source file for the clang-tidy tool [\#139](https://github.com/fktn-k/fkYAML/pull/139) ([fktn-k](https://github.com/fktn-k))
- \#135 optimize build scripts [\#138](https://github.com/fktn-k/fkYAML/pull/138) ([fktn-k](https://github.com/fktn-k))
- \#132 Classify source files in detail directory [\#137](https://github.com/fktn-k/fkYAML/pull/137) ([fktn-k](https://github.com/fktn-k))
- \#126 Generalize serialization/deserialization features [\#134](https://github.com/fktn-k/fkYAML/pull/134) ([fktn-k](https://github.com/fktn-k))
- \#128 moved internal impl to detail dir/namespace [\#131](https://github.com/fktn-k/fkYAML/pull/131) ([fktn-k](https://github.com/fktn-k))
- \#127 Generalize conversions between nodes and native data [\#129](https://github.com/fktn-k/fkYAML/pull/129) ([fktn-k](https://github.com/fktn-k))
- \#123 Changed file naming convention [\#125](https://github.com/fktn-k/fkYAML/pull/125) ([fktn-k](https://github.com/fktn-k))
- \#123 Change naming conventions to lower\_snake\_case [\#124](https://github.com/fktn-k/fkYAML/pull/124) ([fktn-k](https://github.com/fktn-k))

## [v0.0.1](https://github.com/fktn-k/fkYAML/releases/tag/v0.0.1) (2023-10-06)

[Full Changelog](https://github.com/fktn-k/fkYAML/compare/v0.0.0...v0.0.1)

- \#120 added target to use github-changelog-generator tool [\#121](https://github.com/fktn-k/fkYAML/pull/121) ([fktn-k](https://github.com/fktn-k))
- \#118 introduce cmake-format [\#119](https://github.com/fktn-k/fkYAML/pull/119) ([fktn-k](https://github.com/fktn-k))
- \#116 use reuse software for file headers [\#117](https://github.com/fktn-k/fkYAML/pull/117) ([fktn-k](https://github.com/fktn-k))
- Expand usable key types [\#115](https://github.com/fktn-k/fkYAML/pull/115) ([fktn-k](https://github.com/fktn-k))
- \#113 Generate API documentation only for public members [\#114](https://github.com/fktn-k/fkYAML/pull/114) ([fktn-k](https://github.com/fktn-k))
- \#111 Use docker images for Clang compilers during CI jobs [\#112](https://github.com/fktn-k/fkYAML/pull/112) ([fktn-k](https://github.com/fktn-k))
- \#109 Use official docker image for GCC during CI jobs [\#110](https://github.com/fktn-k/fkYAML/pull/110) ([fktn-k](https://github.com/fktn-k))
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
