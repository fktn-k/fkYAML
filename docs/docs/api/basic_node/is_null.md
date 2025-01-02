<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_null

```cpp
bool is_null() const noexcept;
```

Tests whether the node is a null node.  

## **Return Value**

`true` if the node is a null node, `false` otherwise.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/is_null.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/is_null.output"
    ```

## **See Also**

* [node_type](../node_type.md)
