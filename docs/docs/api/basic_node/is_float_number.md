<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_float_number

```cpp
bool is_float_number() const noexcept;
```

Tests whether the node is a floating point value node.  

## **Return Value**

`true` if the node is a floating point value node, `false` otherwise.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/is_float_number.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/is_float_number.output"
    ```

## **See Also**

* [node_type](../node_type.md)
