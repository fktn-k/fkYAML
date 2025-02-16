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

## **Template Paramters**

| Template parameter | Description                                                  | Default                                                              | Derived type                                      |
|--------------------|--------------------------------------------------------------|----------------------------------------------------------------------|---------------------------------------------------|
| `SequenceType`     | type for sequence node value containers                      | [std::vector](https://en.cppreference.com/w/cpp/container/vector)    | [`sequence_type`](sequence_type.md)               |
| `MappingType`      | type for mapping node value containers                       | [std::map](https://en.cppreference.com/w/cpp/container/map)          | [`mapping_type`](mapping_type.md)                 |
| `BooleanType`      | type for boolean node values                                 | [bool](https://en.cppreference.com/w/cpp/keyword/bool)               | [`boolean_type`](boolean_type.md)                 |
| `IntegerType`      | type for integer node values                                 | [std::int64_t](https://en.cppreference.com/w/cpp/types/integer)      | [`integer_type`](integer_type.md)                 |
| `FloatNumberType`  | type for float number node values                            | [double](https://en.cppreference.com/w/cpp/keyword/double)           | [`float_number_type`](float_number_type.md)       |
| `StringType`       | type for string node values                                  | [std::string](https://en.cppreference.com/w/cpp/string/basic_string) | [`string_type`](string_type.md)                   |
| `ConverterType`    | type for converters between <br> nodes and native data types | [node_value_converter](../node_value_converter/index.md)             | [`value_converter_type`](value_converter_type.md) |

## **Specializations**

* [node](node.md) - default specialization

## **Member Types**

### Node Value Types
| Name                                      | Description                                            |
| ----------------------------------------- | ------------------------------------------------------ |
| [sequence_type](sequence_type.md)         | The type used to store sequence node value containers. |
| [mapping_type](mapping_type.md)           | The type used to store mapping node value containers.  |
| [boolean_type](boolean_type.md)           | The type used to store boolean node values.            |
| [integer_type](integer_type.md)           | The type used to store integer node values.            |
| [float_number_type](float_number_type.md) | The type used to store float number node values.       |
| [string_type](string_type.md)             | The type used to store string node values.             |

### Container Types
| Name                                          | Description                                                                                               |
| --------------------------------------------- | --------------------------------------------------------------------------------------------------------- |
| value_type                                    | `basic_node`                                                                                              |
| reference                                     | `basic_node&`                                                                                             |
| const_reference                               | `const basic_node&`                                                                                       |
| pointer                                       | `basic_node*`                                                                                             |
| const_pointer                                 | `const basic_node*`                                                                                       |
| size_type                                     | [`std::size_t`](https://en.cppreference.com/w/cpp/types/size_t)                                           |
| difference_type                               | [`std::ptrdiff_t`](https://en.cppreference.com/w/cpp/types/ptrdiff_t)                                     |
| [iterator](iterator.md)                       | [LegacyBidirectionalIterator](https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator)          |
| [const_iterator](iterator.md)                 | constant [LegacyBidirectionalIterator](https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator) |
| [reverse_iterator](reverse_iterator.md)       | reverse iterator derived from `iterator`          |
| [const_reverse_iterator](reverse_iterator.md) | reverse iterator derived from `const_iterator` |

### Miscellaneous
| Name                                            | Description                                                         |
| ----------------------------------------------- | ------------------------------------------------------------------- |
| [value_converter_type](value_converter_type.md) | The type used to convert between node and native data.              |
| initializer_list_t                              | The type for initializer lists of `basic_node` values.              |
| [node_t](node_t.md)                             | **(DEPRECATED)** The type used to store the internal value type.    |
| [yaml_version_t](yaml_version_t.md)             | **(DEPRECATED)** The type used to store the enable version of YAML. |
| [map_range](map_range.md)                       | The helper type for the `map_items()` function.                     |
| [const_map_range](map_range.md)                 | The helper type for the `map_items()` function.                     |

## **Member Functions**

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
| Name                                  | Description                                                        |
| ------------------------------------- | ------------------------------------------------------------------ |
| [get_type](get_type.md)               | returns the type of a node value in a basic_node.                  |
| [type](type.md)                       | **(DEPRECATED)** returns the type of a node value in a basic_node. |
| [is_sequence](is_sequence.md)         | checks if a basic_node has a sequence node value.                  |
| [is_mapping](is_mapping.md)           | checks if a basic_node has a mapping node value.                   |
| [is_null](is_null.md)                 | checks if a basic_node has a null node value.                      |
| [is_scalar](is_scalar.md)             | checks if a basic_node has a scalar node value.                    |
| [is_boolean](is_boolean.md)           | checks if a basic_node has a boolean node value.                   |
| [is_integer](is_integer.md)           | checks if a basic_node has an integer node value.                  |
| [is_float_number](is_float_number.md) | checks if a basic_node has a float number node value.              |
| [is_string](is_string.md)             | checks if a basic_node has a string node value.                    |

### Conversions
| Name                                      |          | Description                                                                                       |
| ----------------------------------------- | -------- | ------------------------------------------------------------------------------------------------- |
| [deserialize](deserialize.md)             | (static) | deserializes the first YAML document into a basic_node.                                           |
| [deserialize_docs](deserialize_docs.md)   | (static) | deserializes all YAML documents into basic_node objects.                                          |
| [operator>>](extraction_operator.md)      |          | deserializes an input stream into a basic_node.                                                   |
| [serialize](serialize.md)                 | (static) | serializes a basic_node into a YAML formatted string.                                             |
| [serialize_docs](serialize_docs.md)       | (static) | serializes basic_node objects into a YAML formatted string.                                       |
| [operator<<](insertion_operator.md)       |          | serializes a basic_node into an output stream.                                                    |
| [get_value](get_value.md)                 |          | converts a basic_node into a target type.                                                         |
| [get_value_inplace](get_value_inplace.md) |          | converts a basic_node into a target type and write it to a destination.                           |
| [get_value_or](get_value_or.md)           |          | tries to convert a basic_node into a target type.<br>returns a default value if conversion fails. |
| [as_seq](as_seq.md)                       |          | get reference to the sequence node value.                                                         |
| [as_map](as_map.md)                       |          | get reference to the mapping node value.                                                          |
| [as_bool](as_bool.md)                     |          | get reference to the boolean node value.                                                          |
| [as_int](as_int.md)                       |          | get reference to the integer node value.                                                          |
| [as_float](as_float.md)                   |          | get reference to the float node value.                                                            |
| [as_str](as_str.md)                       |          | get reference to the string node value.                                                           |
| [get_value_ref](get_value_ref.md)         |          | **(DEPRECATED)** converts a basic_node into reference to a target type.                           |

### Iterators
| Name                      | Description                                                                                                |
| ------------------------- | ---------------------------------------------------------------------------------------------------------- |
| [begin](begin.md)         | returns a (const) iterator to the beginning of mapping/sequence                                            |
| [cbegin](begin.md)        | returns a const iterator to the beginning of mapping/sequence                                              |
| [end](end.md)             | returns a (const) iterator to the past-the-last of mapping/sequence                                        |
| [cend](end.md)            | returns a const iterator to the past-the-last of mapping/sequence                                          |
| [rbegin](rbegin.md)       | returns a (const) iterator to the first of reversed mapping/sequence                                       |
| [crbegin](rbegin.md)      | returns a const iterator to the first of reversed mapping/sequence                                         |
| [rend](rend.md)           | returns a (const) iterator to the past-the-last of reversed mapping/sequence                               |
| [crend](rend.md)          | returns a const iterator to the past-the-last of reversed mapping/sequence                                 |
| [map_items](map_items.md) | returns a range of mapping entries, allowing access to iterator member functions in range-based for loops. |

### Inspection for Container Node Values
| Name                    | Description                                            |
|-------------------------|--------------------------------------------------------|
| [contains](contains.md) | checks if a basic_node has the given key.              |
| [empty](empty.md)       | checks if a basic_node has an empty container.         |
| [size](size.md)         | returns the size of a container value of a basic_node. |

### Accessors for Container Elements
| Name                        | Description                                                       |
| --------------------------- | ----------------------------------------------------------------- |
| [operator[]](operator[].md) | accesses an item specified by the key/index.                      |
| [at](at.md)                 | accesses an item specified by the key/index with bounds checking. |

### Lexicographical Comparison Operators
| Name                         | Description                       |
| ---------------------------- | --------------------------------- |
| [operator==](operator_eq.md) | comparison: equal                 |
| [operator!=](operator_ne.md) | comparison: not equal             |
| [operator<](operator_lt.md)  | comparison: less than             |
| [operator<=](operator_le.md) | comparison: less than or equal    |
| [operator>](operator_gt.md)  | comparison: greater than          |
| [operator>=](operator_ge.md) | comparison: greater than or equal |

### Manipulations for Node Properties
| Name                                  | Description                                              |
| ------------------------------------- | -------------------------------------------------------- |
| [is_alias](is_alias.md)               | checks if a basic_node is an alias node.                 |
| [is_anchor](is_anchor.md)             | checks if a basic_node is an anchor node.                |
| [add_anchor_name](add_anchor_name.md) | registers an anchor name to a basic_node object.         |
| [get_anchor_name](get_anchor_name.md) | gets an anchor name associated with a basic_node object. |
| [has_anchor_name](has_anchor_name.md) | checks if a basic_node has any anchor name.              |
| [add_tag_name](add_tag_name.md)       | registers a tag name to a basic_node object.             |
| [get_tag_name](get_tag_name.md)       | gets a tag name associated with a basic_node object.     |
| [has_tag_name](has_tag_name.md)       | checks if a basic_node has any tag name.                 |

### Manipulations for Document Properties
| Name                                              | Description                                                               |
| ------------------------------------------------- | ------------------------------------------------------------------------- |
| [get_yaml_version_type](get_yaml_version_type.md) | gets a YAML version associated with a basic_node object.                  |
| [set_yaml_version_type](set_yaml_version_type.md) | sets a YAML version to a basic_node object.                               |
| [get_yaml_version](get_yaml_version.md)           | **(DEPRECATED)** gets a YAML version associated with a basic_node object. |
| [set_yaml_version](set_yaml_version.md)           | **(DEPRECATED)** sets a YAML version to a basic_node object.              |

### Modifiers

| Name            | Description                      |
| --------------- | -------------------------------- |
| [swap](swap.md) | swaps the internally stored data |
