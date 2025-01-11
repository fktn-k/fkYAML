<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_scalar

```cpp
bool is_scalar() const noexcept;
```

Tests whether the node is a scalar node: either a null, boolean, integer, floating point or string node.

## **Return Value**

`true` if the node is a scalar node, `false` otherwise.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/is_scalar.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/is_scalar.output"
    ```

## **See Also**

* [node_type](../node_type.md)
