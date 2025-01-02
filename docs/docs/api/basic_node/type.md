<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>type

```cpp
node_t type() const noexcept;
```

!!! warning "Deprecation"

    The function [`#!cpp node_type get_type()`](get_type.md) replaces the function `basic_node::node_t type()` which has been deprecated in version 0.3.12. It will be removed in a future version. Please replace calls like  
    
    ```cpp
    fkyaml::node::node_t t = n.type();
    ```
    
    with  
    
    ```cpp
    fkyaml::node_type t = n.get_type();
    ```

Returns the type of the YAML node value.  

## **Return Value**

The type of the YAML node value.

| Value Type            | Return Value         |
| --------------------- | -------------------- |
| sequence              | node_t::SEQUENCE     |
| mapping               | node_t::MAPPING      |
| null                  | node_t::NULL_OBJECT  |
| boolean               | node_t::BOOLEAN      |
| integer               | node_t::INTEGER      |
| floating point number | node_t::FLOAT_NUMBER |
| string                | node_t::STRING       |

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/type.output"
    ```

## **See Also**

* [node_t](node_t.md)
