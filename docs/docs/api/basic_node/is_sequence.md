<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_sequence

```cpp
bool is_sequence() const noexcept;
```

Tests whether the node is a sequence node.  

## **Return Value**

`true` if the node is a sequence node, `false` otherwise.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/is_sequence.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/is_sequence.output"
    ```

## **See Also**

* [node_type](../node_type.md)
