# yaml_test_suite Failure Report

Command: `ctest --test-dir build_yaml_test_suite --output-on-failure -R 'yaml_test_suite_'`

Failed cases: 13

| case id | format | input file | label | failure |
| --- | --- | --- | --- | --- |
| 5U3A | YAML | [5U3A/in.yaml](#input-data-5u3a-yaml) | Sequence on same Line as Mapping Key | test_deserialize.cpp:174: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| 6CK3 | YAML | [6CK3/in.yaml](#input-data-6ck3-yaml) | Spec Example 6.26. Tag Shorthands | test_deserialize.cpp:186: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Tag validation failed: expected tag:example.com,2000:app/tag!, got tag:example.com,2000:app/tag%21" |
| CT4Q | YAML | [CT4Q/in.yaml](#input-data-ct4q-yaml) | Spec Example 7.20. Single Pair Explicit Entry | test_deserialize.cpp:182: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected invalid indentation. (at line 1, column 0)" |
| CXX2 | YAML | [CXX2/in.yaml](#input-data-cxx2-yaml) | Mapping with anchor on document start line | test_deserialize.cpp:174: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| DFF7 | YAML | [DFF7/in.yaml](#input-data-dff7-yaml) | Spec Example 7.16. Flow Mapping Entries | test_deserialize.cpp:182: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected invalid indentation. (at line 1, column 0)" |
| FRK4 | YAML | [FRK4/in.yaml](#input-data-frk4-yaml) | Spec Example 7.3. Completely Empty Flow Nodes | test_deserialize.cpp:182: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: No corresponding flow mapping beginning is found. (at line 3, column 0)" |
| G9HC | YAML | [G9HC/in.yaml](#input-data-g9hc-yaml) | Invalid anchor in zero indented sequence | test_deserialize.cpp:174: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| GT5M | YAML | [GT5M/in.yaml](#input-data-gt5m-yaml) | Node anchor in sequence | test_deserialize.cpp:174: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| JEF9-02 | YAML | [JEF9/02/in.yaml](#input-data-jef9-02-yaml) | Trailing whitespace in streams | test_deserialize.cpp:186: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected |
| L24T-01 | YAML | [L24T/01/in.yaml](#input-data-l24t-01-yaml) | Trailing line of spaces | test_deserialize.cpp:186: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: Value validation failed: expected x |
| M5C3 | YAML | [M5C3/in.yaml](#input-data-m5c3-yaml) | Spec Example 8.21. Block Scalar Nodes | test_deserialize.cpp:182: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 6, column 0)" |
| QB6E | YAML | [QB6E/in.yaml](#input-data-qb6e-yaml) | Wrong indented multiline quoted scalar | test_deserialize.cpp:174: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| SY6V | YAML | [SY6V/in.yaml](#input-data-sy6v-yaml) | Anchor before sequence entry on same line | test_deserialize.cpp:174: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |

## Input Data

<a id="input-data-5u3a-yaml"></a>
### 5U3A (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `5U3A/in.yaml`

```yaml
key: - a
     - b

```

Error:
```
Error: test_deserialize.cpp:174: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-6ck3-yaml"></a>
### 6CK3 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `6CK3/in.yaml`

```yaml
%TAG !e! tag:example.com,2000:app/
---
- !local foo
- !!str bar
- !e!tag%21 baz

```

Error:
```
Error: test_deserialize.cpp:186: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Tag validation failed: expected tag:example.com,2000:app/tag!, got tag:example.com,2000:app/tag%21"
```

<a id="input-data-ct4q-yaml"></a>
### CT4Q (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `CT4Q/in.yaml`

```yaml
[
? foo
 bar : baz
]

```

Error:
```
Error: test_deserialize.cpp:182: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected invalid indentation. (at line 1, column 0)"
```

<a id="input-data-cxx2-yaml"></a>
### CXX2 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `CXX2/in.yaml`

```yaml
--- &anchor a: b

```

Error:
```
Error: test_deserialize.cpp:174: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-dff7-yaml"></a>
### DFF7 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `DFF7/in.yaml`

```yaml
{
? explicit: entry,
implicit: entry,
?
}

```

Error:
```
Error: test_deserialize.cpp:182: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected invalid indentation. (at line 1, column 0)"
```

<a id="input-data-frk4-yaml"></a>
### FRK4 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `FRK4/in.yaml`

```yaml
{
  ? foo :,
  : bar,
}

```

Error:
```
Error: test_deserialize.cpp:182: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: No corresponding flow mapping beginning is found. (at line 3, column 0)"
```

<a id="input-data-g9hc-yaml"></a>
### G9HC (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `G9HC/in.yaml`

```yaml
---
seq:
&anchor
- a
- b

```

Error:
```
Error: test_deserialize.cpp:174: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-gt5m-yaml"></a>
### GT5M (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `GT5M/in.yaml`

```yaml
- item1
&node
- item2

```

Error:
```
Error: test_deserialize.cpp:174: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-jef9-02-yaml"></a>
### JEF9-02 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `JEF9/02/in.yaml`

```yaml
- |+
   
```

Error:
```
Error: test_deserialize.cpp:186: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected
```

<a id="input-data-l24t-01-yaml"></a>
### L24T-01 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `L24T/01/in.yaml`

```yaml
foo: |
  x
   
```

Error:
```
Error: test_deserialize.cpp:186: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: Value validation failed: expected x
```

<a id="input-data-m5c3-yaml"></a>
### M5C3 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `M5C3/in.yaml`

```yaml
literal: |2
  value
folded:
   !foo
  >1
 value

```

Error:
```
Error: test_deserialize.cpp:182: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 6, column 0)"
```

<a id="input-data-qb6e-yaml"></a>
### QB6E (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `QB6E/in.yaml`

```yaml
---
quoted: "a
b
c"

```

Error:
```
Error: test_deserialize.cpp:174: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-sy6v-yaml"></a>
### SY6V (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `SY6V/in.yaml`

```yaml
&anchor - sequence entry

```

Error:
```
Error: test_deserialize.cpp:174: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

