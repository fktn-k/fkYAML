<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_type

```cpp
node_type get_type() const noexcept;
```

Returns the type of the node value.  

## **Return Value**

The type of the YAML node value.

| Value Type            | Return Value           |
| --------------------- | ---------------------- |
| sequence              | node_type::SEQUENCE    |
| mapping               | node_type::MAPPING     |
| null                  | node_type::NULL_OBJECT |
| boolean               | node_type::BOOLEAN     |
| integer               | node_type::INTEGER     |
| floating point number | node_type::FLOAT       |
| string                | node_type::STRING      |

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/get_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/get_type.output"
    ```

## **See Also**

* [node_type](../node_type.md)
