<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_string

```cpp
bool is_string() const noexcept;
```

Tests whether the node is a string node.  

## **Return Value**

`true` if the node is a string node, `false` otherwise.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/is_string.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/is_string.output"
    ```

## **See Also**

* [node_type](../node_type.md)
