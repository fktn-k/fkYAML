<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_mapping

```cpp
bool is_mapping() const noexcept;
```

Tests whether the node is a mapping node.  

## **Return Value**

`true` if the node is a mapping node, `false` otherwise.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/is_mapping.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/is_mapping.output"
    ```

## **See Also**

* [node_type](../node_type.md)
