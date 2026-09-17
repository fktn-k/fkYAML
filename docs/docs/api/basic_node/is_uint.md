<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_uint

```cpp
bool is_uint() const noexcept;
```

Tests whether the node value is an unsigned integer.  

## **Return Value**

`true` if the node value is an unsigned integer, `false` otherwise.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/is_uint.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/is_uint.output"
    ```

## **See Also**

* [node_type](../node_type.md)
