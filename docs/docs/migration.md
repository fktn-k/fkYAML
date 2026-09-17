# Migration Guide

This page describes the migration from APIs that were deprecated in previous releases and have now been removed.  
The replacements are available in the
current API and do not require compatibility wrappers.

## Quick reference

| Removed API                                    | Replacement                                            | Decprecated since | Removed at |
| ---------------------------------------------- | ------------------------------------------------------ | ----------------- | ---------- |
| `basic_node::node_t`                           | `fkyaml::node_type`                                    | v0.3.12           | v0.5.0     |
| `basic_node::yaml_version_t`                   | `fkyaml::yaml_version_type`                            | v0.3.12           | v0.5.0     |
| `basic_node(node_t)`                           | `basic_node(node_type)`                                | v0.3.12           | v0.5.0     |
| `basic_node::type()`                           | `basic_node::get_type()`                               | v0.3.12           | v0.5.0     |
| `basic_node::get_yaml_version()`               | `basic_node::get_yaml_version_type()`                  | v0.3.12           | v0.5.0     |
| `basic_node::set_yaml_version(yaml_version_t)` | `basic_node::set_yaml_version_type(yaml_version_type)` | v0.3.12           | v0.5.0     |
| `basic_node::get_value_ref<T>()`               | The type-specific `as_*()` accessor                    | v0.4.3            | v0.5.0     |

The old `node_t` and `yaml_version_t` types were implementation-detail types.  
They have been replaced by the public `node_type` and `yaml_version_type` enumerations.  
Update both the type name and the enumerator name when migrating.  

## Node types

Replace `node_t` with `node_type` and use the corresponding public enumerators:

| Removed enumerator     | Replacement enumerator   |
| ---------------------- | ------------------------ |
| `node_t::SEQUENCE`     | `node_type::SEQUENCE`    |
| `node_t::MAPPING`      | `node_type::MAPPING`     |
| `node_t::NULL_OBJECT`  | `node_type::NULL_OBJECT` |
| `node_t::BOOLEAN`      | `node_type::BOOLEAN`     |
| `node_t::INTEGER`      | `node_type::INTEGER`     |
| `node_t::FLOAT_NUMBER` | `node_type::FLOAT`       |
| `node_t::STRING`       | `node_type::STRING`      |

```cpp
// Before
fkyaml::node node(fkyaml::node::node_t::SEQUENCE);

// After
fkyaml::node node(fkyaml::node_type::SEQUENCE);
```

The `node_type` definition is documented in [node_type](api/node_type.md).

## Inspecting a node type

Use `get_type()` instead of `type()`. It returns the public `node_type` value directly, so conversion through the removed compatibility type is unnecessary.

```cpp
// Before
if (node.type() == fkyaml::node::node_t::MAPPING) {
    // ...
}

// After
if (node.get_type() == fkyaml::node_type::MAPPING) {
    // ...
}
```

See [get_type](api/basic_node/get_type.md) for the available node types.

## YAML version

Replace `yaml_version_t` with `yaml_version_type`, and use `get_yaml_version_type()` and `set_yaml_version_type()`:

```cpp
// Before
node.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_1);
const auto version = node.get_yaml_version();

// After
node.set_yaml_version_type(fkyaml::yaml_version_type::VERSION_1_1);
const auto version = node.get_yaml_version_type();
```

The public YAML version values are `VERSION_1_1` and `VERSION_1_2`. See
[yaml_version_type](api/yaml_version_type.md),
[get_yaml_version_type](api/basic_node/get_yaml_version_type.md), and
[set_yaml_version_type](api/basic_node/set_yaml_version_type.md).

## Accessing stored values

`get_value_ref<T>()` has been removed. Use the accessor matching the node value
type. These accessors perform the same type check and return a reference to the
stored value:

| Value type            | Accessor     |
| --------------------- | ------------ |
| Sequence              | `as_seq()`   |
| Mapping               | `as_map()`   |
| Boolean               | `as_bool()`  |
| Integer               | `as_int()`   |
| Floating-point number | `as_float()` |
| String                | `as_str()`   |

```cpp
// Before
auto& values = node.get_value_ref<fkyaml::node::sequence_type&>();

// After
auto& values = node.as_seq();
```

For a constant node, the corresponding accessor returns a constant reference:

```cpp
const fkyaml::node node = fkyaml::node::sequence();
const auto& values = node.as_seq();
```

See the [`as_*()` accessors](api/basic_node/as_seq.md) for details and the
complete list of supported value types.
