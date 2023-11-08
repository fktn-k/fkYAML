<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::</small>basic_node

```cpp
template<
    template <typename, typename...> class SequenceType = std::vector,
    template <typename, typename, typename...> class MappingType = std::map,
    typename BooleanType = bool,
    typename IntegerType = std::int64_t,
    typename FloatNumberType = double,
    typename StringType = std::string,
    template <typename, typename = void> class ConverterType = node_value_converter>
class basic_node;
```

This class provides features to handle YAML nodes.

## Template Paramters

| Template parameter | Description                                                  | Default                                                              | Derived type                                      |
|--------------------|--------------------------------------------------------------|----------------------------------------------------------------------|---------------------------------------------------|
| `SequenceType`     | type for sequence node value containers                      | [std::vector](https://en.cppreference.com/w/cpp/container/vector)    | [`sequence_type`](sequence_type.md)               |
| `MappingType`      | type for mapping node value containers                       | [std::map](https://en.cppreference.com/w/cpp/container/map)          | [`mapping_type`](mapping_type.md)                 |
| `BooleanType`      | type for boolean node values                                 | [bool](https://en.cppreference.com/w/cpp/keyword/bool)               | [`boolean_type`](boolean_type.md)                 |
| `IntegerType`      | type for integer node values                                 | [std::int64_t](https://en.cppreference.com/w/cpp/types/integer)      | [`integer_type`](integer_type.md)                 |
| `FloatNumberType`  | type for float number node values                            | [double](https://en.cppreference.com/w/cpp/keyword/double)           | [`float_number_type`](float_number_type.md)       |
| `StringType`       | type for string node values                                  | [std::string](https://en.cppreference.com/w/cpp/string/basic_string) | [`string_type`](string_type.md)                   |
| `ConverterType`    | type for converters between <br> nodes and native data types | [node_value_converter](../node_value_converter/index.md)             | [`value_converter_type`](value_converter_type.md) |

## Specializations

* [node](node.md) - default specialization

## Member Types

| Name                                            | Description                                                |
|-------------------------------------------------|------------------------------------------------------------|
| [boolean_type](boolean_type.md)                 | The type used to store boolean node values.                |
| [const_iterator](const_iterator.md)             | The type for constant iterators.                           |
| [float_number_type](float_number_type.md)       | The type used to store float number node values.           |
| [integer_type](integer_type.md)                 | The type used to store integer node values.                |
| [iterator](iterator.md)                         | The type for non-constant iterators.                       |
| [mapping_type](mapping_type.md)                 | The type used to store mapping node value containers.      |
| [node_t](node_t.md)                             | The type used to store the internal value type.            |
| [sequence_type](sequence_type.md)               | The type used to store sequence node value containers.     |
| [string_type](string_type.md)                   | The type used to store string node values.                 |
| [value_converter_type](value_converter_type.md) | The type used to convert between node and native data.     |
| [yaml_version_t](yaml_version_t.md)             | The type used to store the enable version of YAML.         |

## Member Functions

### Construction/Destruction
| Name                            |          | Description                                                   |
| ------------------------------- | -------- | ------------------------------------------------------------- |
| [(constructor)](constructor.md) |          | constructs a basic_node.                                      |
| [(destructor)](destructor.md)   |          | destroys a basic_node, deallocating internal storage if used. |
| [operator=](operator=.md)       |          | assigns values to the basic_node.                             |
| [mapping](mapping.md)           | (static) | constructs a basic_node with a mapping container.             |
| [sequence](sequence.md)         | (static) | constructs a basic_node with a sequence container.            |
| [alias_of](alias_of.md)         | (static) | constructs a basic_node with an anchor node.                  |

### Inspection for Node Value Types
| Name                                  | Description                                           |
|---------------------------------------|-------------------------------------------------------|
| [type](type.md)                       | returns the type of a node value in a basic_node.     |
| [is_sequence](is_sequence.md)         | checks if a basic_node has a sequence node value.     |
| [is_mapping](is_mapping.md)           | checks if a basic_node has a mapping node value.      |
| [is_null](is_null.md)                 | checks if a basic_node has a null node value.         |
| [is_scalar](is_scalar.md)             | checks if a basic_node has a scalar node value.       |
| [is_boolean](is_boolean.md)           | checks if a basic_node has a boolean node value.      |
| [is_integer](is_integer.md)           | checks if a basic_node has an integer node value.     |
| [is_float_number](is_float_number.md) | checks if a basic_node has a float number node value. |
| [is_string](is_string.md)             | checks if a basic_node has a string node value.       |

### Conversions
| Name                                 |          | Description                                                        |
| ------------------------------------ | -------- | ------------------------------------------------------------------ |
| [deserialize](deserialize.md)        | (static) | deserializes a YAML formatted string into a basic_node.            |
| [operator>>](extraction_operator.md) |          | deserializes an input stream into a basic_node.                    |
| [serialize](serialize.md)            | (static) | serializes a basic_node into a YAML formatted string.              |
| [operator<<](insertion_operator.md)  |          | serializes a basic_node into an output stream.                     |
| [get_value](get_value.md)            |          | converts a basic_node into a target native data type.              |
| [get_value_ref](get_value_ref.md)    |          | converts a basic_node into reference to a target native data type. |

### Iterators
| Name              | Description                                              |
|-------------------|----------------------------------------------------------|
| [begin](begin.md) | returns an iterator to the beginning of mapping/sequence |
| [end](end.md)     | returns an iterator to the end of mapping/sequence       |

### Inspection for Container Node Values
| Name                    | Description                                            |
|-------------------------|--------------------------------------------------------|
| [contains](contains.md) | checks if a basic_node has the given key.              |
| [empty](empty.md)       | checks if a basic_node has an empty container.         |
| [size](size.md)         | returns the size of a container value of a basic_node. |

### Access Elements in Containers
| Name                        | Description                                 |
|-----------------------------|---------------------------------------------|
| [operator[]](operator[].md) | accesses an item specified by the key/index |

### Aliasing Nodes
| Name                                  | Description                                              |
| ------------------------------------- | -------------------------------------------------------- |
| [add_anchor_name](add_anchor_name.md) | registers an anchor name to a basic_node object.         |
| [get_anchor_name](get_anchor_name.md) | gets an anchor name associated with a basic_node object. |
| [has_anchor_name](has_anchor_name.md) | checks if a basic_node has any anchor name.              |

### Modifiers

| Name            | Description                      |
| --------------- | -------------------------------- |
| [swap](swap.md) | swaps the internally stored data |
