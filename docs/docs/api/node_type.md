<small>Defined in header [`<fkYAML/node_type.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node_type.hpp)</small>

# <small>fkyaml::</small>node_type

```cpp
enum class node_type : std::uint32_t
{
    SEQUENCE,
    MAPPING,
    NULL_OBJECT,
    BOOLEAN,
    INTEGER,
    FLOAT,
    STRING,
};
```

This enumeration collects the different YAML value types. They are internally used to distinguish the stored values, and the following [basic_node](basic_node/index.md)'s functions rely on it:

* [`is_sequence()`](basic_node/is_sequence.md)
* [`is_mapping()`](basic_node/is_mapping.md)
* [`is_scalar()`](basic_node/is_scalar.md)
    * [`is_null()`](basic_node/is_null.md)
    * [`is_boolean()`](basic_node/is_boolean.md)
    * [`is_integer()`](basic_node/is_integer.md)
    * [`is_float_number()`](basic_node/is_float_number.md)
    * [`is_string()`](basic_node/is_string.md)

!!! Note "Types of scalars"

    There are five enumerators for scalars (`NULL_OBJECT`, `BOOLEAN`, `INTEGER`, `FLOAT`, `STRING`) to distinguish between different types of scalars:  

    * `std::nullptr_t` for null scalar values
    * [`basic_node::boolean_type`](basic_node/boolean_type.md) for boolean scalar values
    * [`basic_node::integer_type`](basic_node/integer_type.md) for integer scalar values
    * [`basic_node::float_number_type`](basic_node/float_number_type.md) for float number scalar values
    * [`basic_node::string_type`](basic_node/string_type.md) for string scalar values

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/node_type/node_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/node_type/node_type.output"
    ```

## **See Also**

* [basic_node](basic_node/index.md)
  * [get_type](basic_node/get_type.md)
  * [is_sequence](basic_node/is_sequence.md)
  * [is_mapping](basic_node/is_mapping.md)
  * [is_scalar](basic_node/is_scalar.md)
  * [is_null](basic_node/is_null.md)
  * [is_boolean](basic_node/is_boolean.md)
  * [is_integer](basic_node/is_integer.md)
  * [is_float_number](basic_node/is_float_number.md)
  * [is_string](basic_node/is_string.md)
