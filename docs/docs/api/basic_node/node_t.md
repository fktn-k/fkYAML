<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>node_t

```cpp
enum class node_t
{
    SEQUENCE,
    MAPPING,
    NULL_OBJECT,
    BOOLEAN,
    INTEGER,
    FLOAT_NUMBER,
    STRING,
};
```

!!! warning "Deprecation"

    The enum class [`#!cpp fkyaml::node_type`](../node_type.md) replaces the type alias `#!cpp fkyaml::basic_node::node_t` which has been deprecated in version 0.3.12. It will be removed in a future version. Please replace usages like  
    
    ```cpp
    fkyaml::node::node_t t;
    ```

    with  
    
    ```cpp
    fkyaml::node_type t;
    ```

This enumeration collects the different YAML value types.  

!!! Note "Types of scalars"

    There are five enumerators for scalars (`NULL_OBJECT`, `BOOLEAN`, `INTEGER`, `FLOAT_NUMBER`, `STRING`) to distinguish between different types of scalars:  

    * [`boolean_type`](boolean_type.md) for boolean scalar values
    * [`integer_type`](integer_type.md) for integer scalar values
    * [`float_number_type`](float_number_type.md) for float number scalar values
    * [`string_type`](string_type.md) for string scalar values

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/node_t.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/node_t.output"
    ```

## **See Also**

* [type](type.md)
* [is_sequence](is_sequence.md)
* [is_mapping](is_mapping.md)
* [is_scalar](is_scalar.md)
* [is_null](is_null.md)
* [is_boolean](is_boolean.md)
* [is_integer](is_integer.md)
* [is_float_number](is_float_number.md)
* [is_string](is_string.md)
