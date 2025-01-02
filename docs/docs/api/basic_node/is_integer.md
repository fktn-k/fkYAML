<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_integer

```cpp
bool is_integer() const noexcept;
```

Tests whether the node is an integer node.  

## **Return Value**

`true` if the node is an integer node, `false` otherwise.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/is_integer.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/is_integer.output"
    ```

## **See Also**

* [node_type](../node_type.md)
