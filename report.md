# yaml_test_suite Failure Report

Command: `ctest --test-dir build_yaml_test_suite --output-on-failure -R 'yaml_test_suite_'`

Failed cases: 112

| case id | format | input file | label | failure |
| --- | --- | --- | --- | --- |
| 26DV | YAML | [26DV/in.yaml](#input-data-26dv-yaml) | Whitespace around colon in mappings | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Anchor cannot be specified to an alias node. (at line 5, column 2)" |
| 2JQS | YAML | [2JQS/in.yaml](#input-data-2jqs-yaml) | Block Mapping with Missing Keys | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected duplication in mapping keys. (at line 1, column 0)" |
| 35KP | YAML | [35KP/in.yaml](#input-data-35kp-yaml) | Tags for Root Objects | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected d e, got e" |
| 3HFZ | YAML | [3HFZ/in.yaml](#input-data-3hfz-yaml) | Invalid content after document end marker | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| 4ABK | YAML | [4ABK/in.yaml](#input-data-4abk-yaml) | Flow Mapping Separate Values | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 2, column 14)" |
| 4CQQ | YAML | [4CQQ/in.yaml](#input-data-4cqq-yaml) | Spec Example 2.18. Multi-line Flow Scalars | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 4, column 0)" |
| 4EJS | YAML | [4EJS/in.yaml](#input-data-4ejs-yaml) | Invalid tabs as indendation in a mapping | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| 4JVG | YAML | [4JVG/in.yaml](#input-data-4jvg-yaml) | Scalar value with two anchors | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| 4MUZ-00 | YAML | [4MUZ/00/in.yaml](#input-data-4muz-00-yaml) | Flow mapping colon on line after key | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 1, column 0)" |
| 4MUZ-01 | YAML | [4MUZ/01/in.yaml](#input-data-4muz-01-yaml) | Flow mapping colon on line after key | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 1, column 0)" |
| 4MUZ-02 | YAML | [4MUZ/02/in.yaml](#input-data-4muz-02-yaml) | Flow mapping colon on line after key | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 1, column 0)" |
| 5MUD | YAML | [5MUD/in.yaml](#input-data-5mud-yaml) | Colon and adjacent value on next line | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 2, column 2)" |
| 5TRB | YAML | [5TRB/in.yaml](#input-data-5trb-yaml) | Invalid document-start marker in doublequoted tring | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| 5U3A | YAML | [5U3A/in.yaml](#input-data-5u3a-yaml) | Sequence on same Line as Mapping Key | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| 6BFJ | YAML | [6BFJ/in.yaml](#input-data-6bfj-yaml) | Mapping, key and flow sequence item anchors | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: invalid flow sequence beginning is found. (at line 2, column 0)" |
| 6CK3 | YAML | [6CK3/in.yaml](#input-data-6ck3-yaml) | Spec Example 6.26. Tag Shorthands | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Tag validation failed: expected tag:example.com,2000:app/tag!, got tag:example.com,2000:app/tag%21" |
| 6JWB | YAML | [6JWB/in.yaml](#input-data-6jwb-yaml) | Tags for Block Objects | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: A sequence or mapping tag cannot be specified to a scalar node. (at line 2, column 4)" |
| 6KGN | YAML | [6KGN/in.yaml](#input-data-6kgn-yaml) | Anchor for empty node | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: Anchor validation failed: expected an anchor node, but is not" |
| 6M2F | YAML | [6M2F/in.yaml](#input-data-6m2f-yaml) | Aliases in Explicit Block Mapping | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Size validation failed: expected 2, got 1" |
| 6PBE | YAML | [6PBE/in.yaml](#input-data-6pbe-yaml) | Zero-indented sequences in explicit mapping keys | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: invalid block sequence entry is found. (at line 2, column 0)" |
| 6VJK | YAML | [6VJK/in.yaml](#input-data-6vjk-yaml) | Spec Example 2.15. Folded newlines are preserved for "more indented" and blank lines | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected Sammy Sosa completed another fine season with great stats. |
| 6XDY | YAML | [6XDY/in.yaml](#input-data-6xdy-yaml) | Two document start markers | test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct! |
| 735Y | YAML | [735Y/in.yaml](#input-data-735y-yaml) | Spec Example 8.20. Block Node Types | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: A sequence or mapping tag cannot be specified to a scalar node. (at line 4, column 2)" |
| 7BMT | YAML | [7BMT/in.yaml](#input-data-7bmt-yaml) | Node and Mapping Key Anchors [1.3] | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: Anchor validation failed: expected an anchor node, but is not" |
| 7FWL | YAML | [7FWL/in.yaml](#input-data-7fwl-yaml) | Spec Example 6.24. Verbatim Tags | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: no matching key was found." |
| 82AN | YAML | [82AN/in.yaml](#input-data-82an-yaml) | Three dashes and content without space | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected ---word1 word2, got word2" |
| 8G76 | YAML | [8G76/in.yaml](#input-data-8g76-yaml) | Spec Example 6.10. Comment Lines | test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct! |
| 8G76 | JSON | [8G76/in.json](#input-data-8g76-json) | Spec Example 6.10. Comment Lines | test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct! |
| 8KB6 | YAML | [8KB6/in.yaml](#input-data-8kb6-yaml) | Multiline plain flow mapping key without value | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 1, column 15)" |
| 98YD | YAML | [98YD/in.yaml](#input-data-98yd-yaml) | Spec Example 5.5. Comment Indicator | test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct! |
| 98YD | JSON | [98YD/in.json](#input-data-98yd-json) | Spec Example 5.5. Comment Indicator | test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct! |
| 9BXH | YAML | [9BXH/in.yaml](#input-data-9bxh-yaml) | Multiline doublequoted flow mapping key without value | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 1, column 17)" |
| 9C9N | YAML | [9C9N/in.yaml](#input-data-9c9n-yaml) | Wrong indented flow sequence | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| 9HCY | YAML | [9HCY/in.yaml](#input-data-9hcy-yaml) | Need document footer before directives | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| 9KAX | YAML | [9KAX/in.yaml](#input-data-9kax-yaml) | Various combinations of tags and anchors | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Anchor validation failed: expected an anchor node, but is not" |
| 9MQT-01 | YAML | [9MQT/01/in.yaml](#input-data-9mqt-01-yaml) | Scalar doc with '...' in content | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| 9SA2 | YAML | [9SA2/in.yaml](#input-data-9sa2-yaml) | Multiline double quoted flow mapping key | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 3, column 7)" |
| 9YRD | YAML | [9YRD/in.yaml](#input-data-9yrd-yaml) | Multiline Scalar at Top Level | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected a b c d |
| AVM7 | YAML | [AVM7/in.yaml](#input-data-avm7-yaml) | Empty Stream | test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct! |
| AVM7 | JSON | [AVM7/in.json](#input-data-avm7-json) | Empty Stream | test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct! |
| BEC7 | YAML | [BEC7/in.yaml](#input-data-bec7-yaml) | Spec Example 6.14. “YAML” directive | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Only 1.1 and 1.2 can be specified as the YAML version. (at line 0, column 9)" |
| BS4K | YAML | [BS4K/in.yaml](#input-data-bs4k-yaml) | Comment between plain scalar lines | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| BU8L | YAML | [BU8L/in.yaml](#input-data-bu8l-yaml) | Node Anchor and Tag on Seperate Lines | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: A sequence or mapping tag cannot be specified to a scalar node. (at line 1, column 1)" |
| C2SP | YAML | [C2SP/in.yaml](#input-data-c2sp-yaml) | Flow Mapping Key on two lines | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| C4HZ | YAML | [C4HZ/in.yaml](#input-data-c4hz-yaml) | Spec Example 2.24. Global Tags | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected invalid indentation. (at line 5, column 8)" |
| CT4Q | YAML | [CT4Q/in.yaml](#input-data-ct4q-yaml) | Spec Example 7.20. Single Pair Explicit Entry | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected invalid indentation. (at line 1, column 0)" |
| CXX2 | YAML | [CXX2/in.yaml](#input-data-cxx2-yaml) | Mapping with anchor on document start line | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| DE56-01 | YAML | [DE56/01/in.yaml](#input-data-de56-01-yaml) | Trailing tabs in double quoted | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected 2 trailing tab, got 2 trailing tab" |
| DE56-03 | YAML | [DE56/03/in.yaml](#input-data-de56-03-yaml) | Trailing tabs in double quoted | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected 4 trailing tab, got 4 trailing tab" |
| DFF7 | YAML | [DFF7/in.yaml](#input-data-dff7-yaml) | Spec Example 7.16. Flow Mapping Entries | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected invalid indentation. (at line 1, column 0)" |
| DK3J | YAML | [DK3J/in.yaml](#input-data-dk3j-yaml) | Zero indented block scalar with line that looks like a comment | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The first non-empty line in the block scalar is less indented. (at line 0, column 4)" |
| DK4H | YAML | [DK4H/in.yaml](#input-data-dk4h-yaml) | Implicit key followed by newline | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| DK95-01 | YAML | [DK95/01/in.yaml](#input-data-dk95-01-yaml) | Tabs that look like indentation | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| DWX9 | YAML | [DWX9/in.yaml](#input-data-dwx9-yaml) | Spec Example 8.8. Literal Content | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: A content line of the block scalar is less indented. (at line 0, column 0)" |
| EB22 | YAML | [EB22/in.yaml](#input-data-eb22-yaml) | Missing document-end marker before directive | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| EX5H | YAML | [EX5H/in.yaml](#input-data-ex5h-yaml) | Multiline Scalar at Top Level [1.3] | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected a b c d |
| EXG3 | YAML | [EXG3/in.yaml](#input-data-exg3-yaml) | Three dashes and content without space [1.3] | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected ---word1 word2, got word2" |
| F8F9 | YAML | [F8F9/in.yaml](#input-data-f8f9-yaml) | Spec Example 8.5. Chomping Trailing Lines | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: A content line of the block scalar is less indented. (at line 2, column 7)" |
| FH7J | YAML | [FH7J/in.yaml](#input-data-fh7j-yaml) | Tags on Empty Scalars | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Failed to convert a scalar to a null. (at line 2, column 11)" |
| FP8R | YAML | [FP8R/in.yaml](#input-data-fp8r-yaml) | Zero indented block scalar | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The first non-empty line in the block scalar is less indented. (at line 0, column 4)" |
| FRK4 | YAML | [FRK4/in.yaml](#input-data-frk4-yaml) | Spec Example 7.3. Completely Empty Flow Nodes | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: No corresponding flow mapping beginning is found. (at line 3, column 0)" |
| G5U8 | YAML | [G5U8/in.yaml](#input-data-g5u8-yaml) | Plain dashes in flow sequence | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| G9HC | YAML | [G9HC/in.yaml](#input-data-g9hc-yaml) | Invalid anchor in zero indented sequence | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| GT5M | YAML | [GT5M/in.yaml](#input-data-gt5m-yaml) | Node anchor in sequence | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| HS5T | YAML | [HS5T/in.yaml](#input-data-hs5t-yaml) | Spec Example 7.12. Plain Lines | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected 1st non-empty |
| HWV9 | YAML | [HWV9/in.yaml](#input-data-hwv9-yaml) | Document-end marker | test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct! |
| HWV9 | JSON | [HWV9/in.json](#input-data-hwv9-json) | Document-end marker | test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct! |
| JEF9-02 | YAML | [JEF9/02/in.yaml](#input-data-jef9-02-yaml) | Trailing whitespace in streams | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected |
| K3WX | YAML | [K3WX/in.yaml](#input-data-k3wx-yaml) | Colon and adjacent value after comment on next line | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 2, column 2)" |
| K858 | YAML | [K858/in.yaml](#input-data-k858-yaml) | Spec Example 8.6. Empty Scalar Chomping | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The first non-empty line in the block scalar is less indented. (at line 0, column 7)" |
| KK5P | YAML | [KK5P/in.yaml](#input-data-kk5p-yaml) | Various combinations of explicit block mappings | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Size validation failed: expected 5, got 3" |
| L24T-01 | YAML | [L24T/01/in.yaml](#input-data-l24t-01-yaml) | Trailing line of spaces | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: Value validation failed: expected x |
| L94M | YAML | [L94M/in.yaml](#input-data-l94m-yaml) | Tags in Explicit Mapping | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: no matching key was found." |
| LE5A | YAML | [LE5A/in.yaml](#input-data-le5a-yaml) | Spec Example 7.24. Flow Nodes | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Type validation failed: expected STRING, got NULL_OBJECT" |
| LP6E | YAML | [LP6E/in.yaml](#input-data-lp6e-yaml) | Whitespace After Scalars in Flow | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected invalid indentation. (at line 2, column 7)" |
| M2N8-01 | YAML | [M2N8/01/in.yaml](#input-data-m2n8-01-yaml) | Question mark edge cases | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: bad indentation of a mapping entry. (at line 0, column 2)" |
| M5C3 | YAML | [M5C3/in.yaml](#input-data-m5c3-yaml) | Spec Example 8.21. Block Scalar Nodes | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The first non-empty line in the block scalar is less indented. (at line 4, column 2)" |
| M5DY | YAML | [M5DY/in.yaml](#input-data-m5dy-yaml) | Spec Example 2.11. Mapping between Sequences | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: bad indentation of a mapping entry. (at line 6, column 2)" |
| M6YH | YAML | [M6YH/in.yaml](#input-data-m6yh-yaml) | Block sequence indentation | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Size validation failed: expected 3, got 2" |
| M7A3 | YAML | [M7A3/in.yaml](#input-data-m7a3-yaml) | Spec Example 9.3. Bare Documents | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The first non-empty line in the block scalar is less indented. (at line 5, column 0)" |
| MJS9 | YAML | [MJS9/in.yaml](#input-data-mjs9-yaml) | Spec Example 6.7. Block Folding | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected foo |
| NAT4 | YAML | [NAT4/in.yaml](#input-data-nat4-yaml) | Various empty or newline only quoted strings | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: Value validation failed: expected , got " |
| NB6Z | YAML | [NB6Z/in.yaml](#input-data-nb6z-yaml) | Multiline plain value with tabs on empty lines | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 4, column 2)" |
| NJ66 | YAML | [NJ66/in.yaml](#input-data-nj66-yaml) | Multiline plain flow mapping key | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 3, column 6)" |
| PW8X | YAML | [PW8X/in.yaml](#input-data-pw8x-yaml) | Anchors on Empty Scalars | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Size validation failed: expected 6, got 2" |
| QB6E | YAML | [QB6E/in.yaml](#input-data-qb6e-yaml) | Wrong indented multiline quoted scalar | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| QLJ7 | YAML | [QLJ7/in.yaml](#input-data-qlj7-yaml) | Tag shorthand used in documents but only defined in the first | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| QT73 | YAML | [QT73/in.yaml](#input-data-qt73-yaml) | Comment and document-end marker | test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct! |
| QT73 | JSON | [QT73/in.json](#input-data-qt73-json) | Comment and document-end marker | test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct! |
| R4YG | YAML | [R4YG/in.yaml](#input-data-r4yg-yaml) | Spec Example 8.2. Block Indentation Indicator | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected |
| RXY3 | YAML | [RXY3/in.yaml](#input-data-rxy3-yaml) | Invalid document-end marker in single quoted string | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| RZT7 | YAML | [RZT7/in.yaml](#input-data-rzt7-yaml) | Spec Example 2.28. Log File | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 6, column 0)" |
| SM9W-00 | YAML | [SM9W/00/in.yaml](#input-data-sm9w-00-yaml) | Single character streams | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Type validation failed: expected SEQUENCE, got STRING" |
| SY6V | YAML | [SY6V/in.yaml](#input-data-sy6v-yaml) | Anchor before sequence entry on same line | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| T26H | YAML | [T26H/in.yaml](#input-data-t26h-yaml) | Spec Example 8.8. Literal Content [1.3] | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: A content line of the block scalar is less indented. (at line 0, column 4)" |
| U3XV | YAML | [U3XV/in.yaml](#input-data-u3xv-yaml) | Node and Mapping Key Anchors | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: Anchor validation failed: expected an anchor node, but is not" |
| UGM3 | YAML | [UGM3/in.yaml](#input-data-ugm3-yaml) | Spec Example 2.27. Invoice | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 28, column 4)" |
| UKK6-02 | YAML | [UKK6/02/in.yaml](#input-data-ukk6-02-yaml) | Syntax character edge cases | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Type validation failed: expected STRING, got NULL_OBJECT" |
| UT92 | YAML | [UT92/in.yaml](#input-data-ut92-yaml) | Spec Example 9.4. Explicit Documents | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 2, column 0)" |
| VJP3-01 | YAML | [VJP3/01/in.yaml](#input-data-vjp3-01-yaml) | Flow collections over many lines | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 2, column 1)" |
| W4TN | YAML | [W4TN/in.yaml](#input-data-w4tn-yaml) | Spec Example 9.5. Directives Documents | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The first non-empty line in the block scalar is less indented. (at line 1, column 4)" |
| WZ62 | YAML | [WZ62/in.yaml](#input-data-wz62-yaml) | Spec Example 7.2. Empty Content | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Tag shorthand cannot contain flow indicators({}[],). (at line 1, column 14)" |
| X38W | YAML | [X38W/in.yaml](#input-data-x38w-yaml) | Aliases in Flow Objects | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected duplication in mapping keys. (at line 0, column 20)" |
| XLQ9 | YAML | [XLQ9/in.yaml](#input-data-xlq9-yaml) | Multiline scalar that looks like a YAML directive | test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected scalar %YAML 1.2, got %YAML 1.2" |
| Y79Y-001 | YAML | [Y79Y/001/in.yaml](#input-data-y79y-001-yaml) | Tabs in various contexts | test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Any leading empty line must not be more indented than the first non-empty line. (at line 0, column 5)" |
| Y79Y-003 | YAML | [Y79Y/003/in.yaml](#input-data-y79y-003-yaml) | Tabs in various contexts | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| Y79Y-004 | YAML | [Y79Y/004/in.yaml](#input-data-y79y-004-yaml) | Tabs in various contexts | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| Y79Y-005 | YAML | [Y79Y/005/in.yaml](#input-data-y79y-005-yaml) | Tabs in various contexts | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| Y79Y-006 | YAML | [Y79Y/006/in.yaml](#input-data-y79y-006-yaml) | Tabs in various contexts | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| Y79Y-007 | YAML | [Y79Y/007/in.yaml](#input-data-y79y-007-yaml) | Tabs in various contexts | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| Y79Y-008 | YAML | [Y79Y/008/in.yaml](#input-data-y79y-008-yaml) | Tabs in various contexts | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |
| YJV2 | YAML | [YJV2/in.yaml](#input-data-yjv2-yaml) | Dash in flow sequence | test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all! |

## Input Data

<a id="input-data-26dv-yaml"></a>
### 26DV (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `26DV/in.yaml`

```yaml
"top1" : 
  "key1" : &alias1 scalar1
'top2' : 
  'key2' : &alias2 scalar2
top3: &node3 
  *alias1 : scalar3
top4: 
  *alias2 : scalar4
top5   :    
  scalar5
top6: 
  &anchor6 'key6' : scalar6

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Anchor cannot be specified to an alias node. (at line 5, column 2)"
```

<a id="input-data-2jqs-yaml"></a>
### 2JQS (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `2JQS/in.yaml`

```yaml
: a
: b

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected duplication in mapping keys. (at line 1, column 0)"
```

<a id="input-data-35kp-yaml"></a>
### 35KP (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `35KP/in.yaml`

```yaml
--- !!map
? a
: b
--- !!seq
- !!str c
--- !!str
d
e

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected d e, got e"
```

<a id="input-data-3hfz-yaml"></a>
### 3HFZ (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `3HFZ/in.yaml`

```yaml
---
key: value
... invalid

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-4abk-yaml"></a>
### 4ABK (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `4ABK/in.yaml`

```yaml
{
unquoted : "separate",
http://foo.com,
omitted value:,
}

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 2, column 14)"
```

<a id="input-data-4cqq-yaml"></a>
### 4CQQ (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `4CQQ/in.yaml`

```yaml
plain:
  This unquoted scalar
  spans many lines.

quoted: "So does this
  quoted scalar.\n"

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 4, column 0)"
```

<a id="input-data-4ejs-yaml"></a>
### 4EJS (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `4EJS/in.yaml`

```yaml
---
a:
	b:
		c: value

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-4jvg-yaml"></a>
### 4JVG (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `4JVG/in.yaml`

```yaml
top1: &node1
  &k1 key1: val1
top2: &node2
  &v2 val2

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-4muz-00-yaml"></a>
### 4MUZ-00 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `4MUZ/00/in.yaml`

```yaml
{"foo"
: "bar"}

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 1, column 0)"
```

<a id="input-data-4muz-01-yaml"></a>
### 4MUZ-01 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `4MUZ/01/in.yaml`

```yaml
{"foo"
: bar}

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 1, column 0)"
```

<a id="input-data-4muz-02-yaml"></a>
### 4MUZ-02 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `4MUZ/02/in.yaml`

```yaml
{foo
: bar}

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 1, column 0)"
```

<a id="input-data-5mud-yaml"></a>
### 5MUD (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `5MUD/in.yaml`

```yaml
---
{ "foo"
  :bar }

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 2, column 2)"
```

<a id="input-data-5trb-yaml"></a>
### 5TRB (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `5TRB/in.yaml`

```yaml
---
"
---
"

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

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
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-6bfj-yaml"></a>
### 6BFJ (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `6BFJ/in.yaml`

```yaml
---
&mapping
&key [ &item a, b, c ]: value

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: invalid flow sequence beginning is found. (at line 2, column 0)"
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
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Tag validation failed: expected tag:example.com,2000:app/tag!, got tag:example.com,2000:app/tag%21"
```

<a id="input-data-6jwb-yaml"></a>
### 6JWB (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `6JWB/in.yaml`

```yaml
foo: !!seq
  - !!str a
  - !!map
    key: !!str value

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: A sequence or mapping tag cannot be specified to a scalar node. (at line 2, column 4)"
```

<a id="input-data-6kgn-yaml"></a>
### 6KGN (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `6KGN/in.yaml`

```yaml
---
a: &anchor
b: *anchor

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: Anchor validation failed: expected an anchor node, but is not"
```

<a id="input-data-6m2f-yaml"></a>
### 6M2F (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `6M2F/in.yaml`

```yaml
? &a a
: &b b
: *a

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Size validation failed: expected 2, got 1"
```

<a id="input-data-6pbe-yaml"></a>
### 6PBE (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `6PBE/in.yaml`

```yaml
---
?
- a
- b
:
- c
- d

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: invalid block sequence entry is found. (at line 2, column 0)"
```

<a id="input-data-6vjk-yaml"></a>
### 6VJK (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `6VJK/in.yaml`

```yaml
>
 Sammy Sosa completed another
 fine season with great stats.

   63 Home Runs
   0.288 Batting Average

 What a year!

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected Sammy Sosa completed another fine season with great stats.
```

<a id="input-data-6xdy-yaml"></a>
### 6XDY (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `6XDY/in.yaml`

```yaml
---
---

```

Error:
```
Error: test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct!
```

<a id="input-data-735y-yaml"></a>
### 735Y (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `735Y/in.yaml`

```yaml
-
  "flow in block"
- >
 Block scalar
- !!map # Block collection
  foo : bar

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: A sequence or mapping tag cannot be specified to a scalar node. (at line 4, column 2)"
```

<a id="input-data-7bmt-yaml"></a>
### 7BMT (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `7BMT/in.yaml`

```yaml
---
top1: &node1
  &k1 key1: one
top2: &node2 # comment
  key2: two
top3:
  &k3 key3: three
top4: &node4
  &k4 key4: four
top5: &node5
  key5: five
top6: &val6
  six
top7:
  &val7 seven

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: Anchor validation failed: expected an anchor node, but is not"
```

<a id="input-data-7fwl-yaml"></a>
### 7FWL (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `7FWL/in.yaml`

```yaml
!<tag:yaml.org,2002:str> foo :
  !<!bar> baz

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: no matching key was found."
```

<a id="input-data-82an-yaml"></a>
### 82AN (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `82AN/in.yaml`

```yaml
---word1
word2

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected ---word1 word2, got word2"
```

<a id="input-data-8g76-yaml"></a>
### 8G76 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `8G76/in.yaml`

```yaml
  # Comment
   



```

Error:
```
Error: test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct!
```

<a id="input-data-8g76-json"></a>
### 8G76 (JSON)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `8G76/in.json`

```
(empty input)
```

Error:
```
Error: test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct!
```

<a id="input-data-8kb6-yaml"></a>
### 8KB6 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `8KB6/in.yaml`

```yaml
---
- { single line, a: b}
- { multi
  line, a: b}

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 1, column 15)"
```

<a id="input-data-98yd-yaml"></a>
### 98YD (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `98YD/in.yaml`

```yaml
# Comment only.

```

Error:
```
Error: test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct!
```

<a id="input-data-98yd-json"></a>
### 98YD (JSON)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `98YD/in.json`

```
(empty input)
```

Error:
```
Error: test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct!
```

<a id="input-data-9bxh-yaml"></a>
### 9BXH (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `9BXH/in.yaml`

```yaml
---
- { "single line", a: b}
- { "multi
  line", a: b}

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 1, column 17)"
```

<a id="input-data-9c9n-yaml"></a>
### 9C9N (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `9C9N/in.yaml`

```yaml
---
flow: [a,
b,
c]

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-9hcy-yaml"></a>
### 9HCY (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `9HCY/in.yaml`

```yaml
!foo "bar"
%TAG ! tag:example.com,2000:app/
---
!foo "bar"

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-9kax-yaml"></a>
### 9KAX (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `9KAX/in.yaml`

```yaml
---
&a1
!!str
scalar1
---
!!str
&a2
scalar2
---
&a3
!!str scalar3
---
&a4 !!map
&a5 !!str key5: value4
---
a6: 1
&anchor6 b6: 2
---
!!map
&a8 !!str key8: value7
---
!!map
!!str &a10 key10: value9
---
!!str &a11
value11

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Anchor validation failed: expected an anchor node, but is not"
```

<a id="input-data-9mqt-01-yaml"></a>
### 9MQT-01 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `9MQT/01/in.yaml`

```yaml
--- "a
... x
b"

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-9sa2-yaml"></a>
### 9SA2 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `9SA2/in.yaml`

```yaml
---
- { "single line": value}
- { "multi
  line": value}

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 3, column 7)"
```

<a id="input-data-9yrd-yaml"></a>
### 9YRD (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `9YRD/in.yaml`

```yaml
a
b  
  c
d

e

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected a b c d
```

<a id="input-data-avm7-yaml"></a>
### AVM7 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `AVM7/in.yaml`

```
(empty input)
```

Error:
```
Error: test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct!
```

<a id="input-data-avm7-json"></a>
### AVM7 (JSON)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `AVM7/in.json`

```
(empty input)
```

Error:
```
Error: test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct!
```

<a id="input-data-bec7-yaml"></a>
### BEC7 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `BEC7/in.yaml`

```yaml
%YAML 1.3 # Attempt parsing
          # with a warning
---
"foo"

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Only 1.1 and 1.2 can be specified as the YAML version. (at line 0, column 9)"
```

<a id="input-data-bs4k-yaml"></a>
### BS4K (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `BS4K/in.yaml`

```yaml
word1  # comment
word2

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-bu8l-yaml"></a>
### BU8L (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `BU8L/in.yaml`

```yaml
key: &anchor
 !!map
  a: b

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: A sequence or mapping tag cannot be specified to a scalar node. (at line 1, column 1)"
```

<a id="input-data-c2sp-yaml"></a>
### C2SP (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `C2SP/in.yaml`

```yaml
[23
]: 42

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-c4hz-yaml"></a>
### C4HZ (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `C4HZ/in.yaml`

```yaml
%TAG ! tag:clarkevans.com,2002:
--- !shape
  # Use the ! handle for presenting
  # tag:clarkevans.com,2002:circle
- !circle
  center: &ORIGIN {x: 73, y: 129}
  radius: 7
- !line
  start: *ORIGIN
  finish: { x: 89, y: 102 }
- !label
  start: *ORIGIN
  color: 0xFFEEBB
  text: Pretty vector drawing.

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected invalid indentation. (at line 5, column 8)"
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
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected invalid indentation. (at line 1, column 0)"
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
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-de56-01-yaml"></a>
### DE56-01 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `DE56/01/in.yaml`

```yaml
"2 trailing\t  
    tab"

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected 2 trailing tab, got 2 trailing tab"
```

<a id="input-data-de56-03-yaml"></a>
### DE56-03 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `DE56/03/in.yaml`

```yaml
"4 trailing\	  
    tab"

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected 4 trailing tab, got 4 trailing tab"
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
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected invalid indentation. (at line 1, column 0)"
```

<a id="input-data-dk3j-yaml"></a>
### DK3J (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `DK3J/in.yaml`

```yaml
--- >
line1
# no comment
line3

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The first non-empty line in the block scalar is less indented. (at line 0, column 4)"
```

<a id="input-data-dk4h-yaml"></a>
### DK4H (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `DK4H/in.yaml`

```yaml
---
[ key
  : value ]

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-dk95-01-yaml"></a>
### DK95-01 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `DK95/01/in.yaml`

```yaml
foo: "bar
	baz"

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-dwx9-yaml"></a>
### DWX9 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `DWX9/in.yaml`

```yaml
|
 
  
  literal
   
  
  text

 # Comment

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: A content line of the block scalar is less indented. (at line 0, column 0)"
```

<a id="input-data-eb22-yaml"></a>
### EB22 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `EB22/in.yaml`

```yaml
---
scalar1 # comment
%YAML 1.2
---
scalar2

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-ex5h-yaml"></a>
### EX5H (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `EX5H/in.yaml`

```yaml
---
a
b  
  c
d

e

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected a b c d
```

<a id="input-data-exg3-yaml"></a>
### EXG3 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `EXG3/in.yaml`

```yaml
---
---word1
word2

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected ---word1 word2, got word2"
```

<a id="input-data-f8f9-yaml"></a>
### F8F9 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `F8F9/in.yaml`

```yaml
 # Strip
  # Comments:
strip: |-
  # text
  
 # Clip
  # comments:

clip: |
  # text
 
 # Keep
  # comments:

keep: |+
  # text

 # Trail
  # comments.

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: A content line of the block scalar is less indented. (at line 2, column 7)"
```

<a id="input-data-fh7j-yaml"></a>
### FH7J (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `FH7J/in.yaml`

```yaml
- !!str
-
  !!null : a
  b: !!str
- !!str : !!null

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Failed to convert a scalar to a null. (at line 2, column 11)"
```

<a id="input-data-fp8r-yaml"></a>
### FP8R (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `FP8R/in.yaml`

```yaml
--- >
line1
line2
line3

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The first non-empty line in the block scalar is less indented. (at line 0, column 4)"
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
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: No corresponding flow mapping beginning is found. (at line 3, column 0)"
```

<a id="input-data-g5u8-yaml"></a>
### G5U8 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `G5U8/in.yaml`

```yaml
---
- [-, -]

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
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
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
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
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-hs5t-yaml"></a>
### HS5T (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `HS5T/in.yaml`

```yaml
1st non-empty

 2nd non-empty 
	3rd non-empty

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected 1st non-empty
```

<a id="input-data-hwv9-yaml"></a>
### HWV9 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `HWV9/in.yaml`

```yaml
...

```

Error:
```
Error: test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct!
```

<a id="input-data-hwv9-json"></a>
### HWV9 (JSON)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `HWV9/in.json`

```
(empty input)
```

Error:
```
Error: test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct!
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
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected
```

<a id="input-data-k3wx-yaml"></a>
### K3WX (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `K3WX/in.yaml`

```yaml
---
{ "foo" # comment
  :bar }

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 2, column 2)"
```

<a id="input-data-k858-yaml"></a>
### K858 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `K858/in.yaml`

```yaml
strip: >-

clip: >

keep: |+


```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The first non-empty line in the block scalar is less indented. (at line 0, column 7)"
```

<a id="input-data-kk5p-yaml"></a>
### KK5P (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `KK5P/in.yaml`

```yaml
complex1:
  ? - a
complex2:
  ? - a
  : b
complex3:
  ? - a
  : >
    b
complex4:
  ? >
    a
  :
complex5:
  ? - a
  : - b

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Size validation failed: expected 5, got 3"
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
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: Value validation failed: expected x
```

<a id="input-data-l94m-yaml"></a>
### L94M (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `L94M/in.yaml`

```yaml
? !!str a
: !!int 47
? c
: !!str d

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: no matching key was found."
```

<a id="input-data-le5a-yaml"></a>
### LE5A (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `LE5A/in.yaml`

```yaml
- !!str "a"
- 'b'
- &anchor "c"
- *anchor
- !!str

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Type validation failed: expected STRING, got NULL_OBJECT"
```

<a id="input-data-lp6e-yaml"></a>
### LP6E (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `LP6E/in.yaml`

```yaml
- [a, b , c ]
- { "a"  : b
   , c : 'd' ,
   e   : "f"
  }
- [      ]

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected invalid indentation. (at line 2, column 7)"
```

<a id="input-data-m2n8-01-yaml"></a>
### M2N8-01 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `M2N8/01/in.yaml`

```yaml
? []: x

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: bad indentation of a mapping entry. (at line 0, column 2)"
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
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The first non-empty line in the block scalar is less indented. (at line 4, column 2)"
```

<a id="input-data-m5dy-yaml"></a>
### M5DY (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `M5DY/in.yaml`

```yaml
? - Detroit Tigers
  - Chicago cubs
:
  - 2001-07-23

? [ New York Yankees,
    Atlanta Braves ]
: [ 2001-07-02, 2001-08-12,
    2001-08-14 ]

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: bad indentation of a mapping entry. (at line 6, column 2)"
```

<a id="input-data-m6yh-yaml"></a>
### M6YH (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `M6YH/in.yaml`

```yaml
- |
 x
-
 foo: bar
-
 - 42

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Size validation failed: expected 3, got 2"
```

<a id="input-data-m7a3-yaml"></a>
### M7A3 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `M7A3/in.yaml`

```yaml
Bare
document
...
# No document
...
|
%!PS-Adobe-2.0 # Not the first line

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The first non-empty line in the block scalar is less indented. (at line 5, column 0)"
```

<a id="input-data-mjs9-yaml"></a>
### MJS9 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `MJS9/in.yaml`

```yaml
>
  foo 
 
  	 bar

  baz

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected foo
```

<a id="input-data-nat4-yaml"></a>
### NAT4 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `NAT4/in.yaml`

```yaml
---
a: '
  '
b: '  
  '
c: "
  "
d: "  
  "
e: '

  '
f: "

  "
g: '


  '
h: "


  "

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: Value validation failed: expected , got "
```

<a id="input-data-nb6z-yaml"></a>
### NB6Z (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `NB6Z/in.yaml`

```yaml
key:
  value
  with
  	
  tabs

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 4, column 2)"
```

<a id="input-data-nj66-yaml"></a>
### NJ66 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `NJ66/in.yaml`

```yaml
---
- { single line: value}
- { multi
  line: value}

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 3, column 6)"
```

<a id="input-data-pw8x-yaml"></a>
### PW8X (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `PW8X/in.yaml`

```yaml
- &a
- a
-
  &a : a
  b: &b
-
  &c : &a
-
  ? &d
-
  ? &e
  : &a

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Size validation failed: expected 6, got 2"
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
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-qlj7-yaml"></a>
### QLJ7 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `QLJ7/in.yaml`

```yaml
%TAG !prefix! tag:example.com,2011:
--- !prefix!A
a: b
--- !prefix!B
c: d
--- !prefix!C
e: f

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-qt73-yaml"></a>
### QT73 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `QT73/in.yaml`

```yaml
# comment
...

```

Error:
```
Error: test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct!
```

<a id="input-data-qt73-json"></a>
### QT73 (JSON)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `QT73/in.json`

```
(empty input)
```

Error:
```
Error: test_deserialize.cpp:159: FATAL ERROR: REQUIRE( docs.size() == validators.size() ) is NOT correct!
```

<a id="input-data-r4yg-yaml"></a>
### R4YG (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `R4YG/in.yaml`

```yaml
- |
 detected
- >
 
  
  # detected
- |1
  explicit
- >
 	
 detected

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected
```

<a id="input-data-rxy3-yaml"></a>
### RXY3 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `RXY3/in.yaml`

```yaml
---
'
...
'

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-rzt7-yaml"></a>
### RZT7 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `RZT7/in.yaml`

```yaml
---
Time: 2001-11-23 15:01:42 -5
User: ed
Warning:
  This is an error message
  for the log file
---
Time: 2001-11-23 15:02:31 -5
User: ed
Warning:
  A slightly different error
  message.
---
Date: 2001-11-23 15:03:17 -5
User: ed
Fatal:
  Unknown variable "bar"
Stack:
  - file: TopClass.py
    line: 23
    code: |
      x = MoreObject("345\n")
  - file: MoreClass.py
    line: 58
    code: |-
      foo = bar

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 6, column 0)"
```

<a id="input-data-sm9w-00-yaml"></a>
### SM9W-00 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `SM9W/00/in.yaml`

```yaml
-
```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Type validation failed: expected SEQUENCE, got STRING"
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
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-t26h-yaml"></a>
### T26H (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `T26H/in.yaml`

```yaml
--- |
 
  
  literal
   
  
  text

 # Comment

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: A content line of the block scalar is less indented. (at line 0, column 4)"
```

<a id="input-data-u3xv-yaml"></a>
### U3XV (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `U3XV/in.yaml`

```yaml
---
top1: &node1
  &k1 key1: one
top2: &node2 # comment
  key2: two
top3:
  &k3 key3: three
top4:
  &node4
  &k4 key4: four
top5:
  &node5
  key5: five
top6: &val6
  six
top7:
  &val7 seven

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Mapping entry validation failed: Anchor validation failed: expected an anchor node, but is not"
```

<a id="input-data-ugm3-yaml"></a>
### UGM3 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `UGM3/in.yaml`

```yaml
--- !<tag:clarkevans.com,2002:invoice>
invoice: 34843
date   : 2001-01-23
bill-to: &id001
    given  : Chris
    family : Dumars
    address:
        lines: |
            458 Walkman Dr.
            Suite #292
        city    : Royal Oak
        state   : MI
        postal  : 48046
ship-to: *id001
product:
    - sku         : BL394D
      quantity    : 4
      description : Basketball
      price       : 450.00
    - sku         : BL4438H
      quantity    : 1
      description : Super Hoop
      price       : 2392.00
tax  : 251.42
total: 4443.52
comments:
    Late afternoon is best.
    Backup contact is Nancy
    Billsmer @ 338-4338.

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 28, column 4)"
```

<a id="input-data-ukk6-02-yaml"></a>
### UKK6-02 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `UKK6/02/in.yaml`

```yaml
!

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Type validation failed: expected STRING, got NULL_OBJECT"
```

<a id="input-data-ut92-yaml"></a>
### UT92 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `UT92/in.yaml`

```yaml
---
{ matches
% : 20 }
...
---
# Empty
...

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 2, column 0)"
```

<a id="input-data-vjp3-01-yaml"></a>
### VJP3-01 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `VJP3/01/in.yaml`

```yaml
k: {
 k
 :
 v
 }

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The ":" mapping value indicator must be followed after a mapping key. (at line 2, column 1)"
```

<a id="input-data-w4tn-yaml"></a>
### W4TN (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `W4TN/in.yaml`

```yaml
%YAML 1.2
--- |
%!PS-Adobe-2.0
...
%YAML 1.2
---
# Empty
...

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: The first non-empty line in the block scalar is less indented. (at line 1, column 4)"
```

<a id="input-data-wz62-yaml"></a>
### WZ62 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `WZ62/in.yaml`

```yaml
{
  foo : !!str,
  !!str : bar,
}

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Tag shorthand cannot contain flow indicators({}[],). (at line 1, column 14)"
```

<a id="input-data-x38w-yaml"></a>
### X38W (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `X38W/in.yaml`

```yaml
{ &a [a, &b b]: *b, *a : [c, *b, d]}

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Detected duplication in mapping keys. (at line 0, column 20)"
```

<a id="input-data-xlq9-yaml"></a>
### XLQ9 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `XLQ9/in.yaml`

```yaml
---
scalar
%YAML 1.2

```

Error:
```
Error: test_deserialize.cpp:162: ERROR: CHECK_NOTHROW( validators[index]->validate(docs[index]) ) THREW exception: "Value validation failed: expected scalar %YAML 1.2, got %YAML 1.2"
```

<a id="input-data-y79y-001-yaml"></a>
### Y79Y-001 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `Y79Y/001/in.yaml`

```yaml
foo: |
 	
bar: 1

```

Error:
```
Error: test_deserialize.cpp:158: FATAL ERROR: REQUIRE_NOTHROW( docs = fkyaml::node::deserialize_docs(ifs) ) THREW exception: "parse_error: Any leading empty line must not be more indented than the first non-empty line. (at line 0, column 5)"
```

<a id="input-data-y79y-003-yaml"></a>
### Y79Y-003 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `Y79Y/003/in.yaml`

```yaml
- [
	foo,
 foo
 ]

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-y79y-004-yaml"></a>
### Y79Y-004 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `Y79Y/004/in.yaml`

```yaml
-	-

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-y79y-005-yaml"></a>
### Y79Y-005 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `Y79Y/005/in.yaml`

```yaml
- 	-

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-y79y-006-yaml"></a>
### Y79Y-006 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `Y79Y/006/in.yaml`

```yaml
?	-

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-y79y-007-yaml"></a>
### Y79Y-007 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `Y79Y/007/in.yaml`

```yaml
? -
:	-

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-y79y-008-yaml"></a>
### Y79Y-008 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `Y79Y/008/in.yaml`

```yaml
?	key:

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

<a id="input-data-yjv2-yaml"></a>
### YJV2 (YAML)
[Back to failed cases table](#yaml_test_suite-failure-report)
Input file: `YJV2/in.yaml`

```yaml
[-]

```

Error:
```
Error: test_deserialize.cpp:150: ERROR: CHECK_THROWS_AS( fkyaml::node::deserialize_docs(ifs), fkyaml::exception ) did NOT throw at all!
```

