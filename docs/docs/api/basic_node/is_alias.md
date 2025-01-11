<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_alias

```cpp
bool is_alias() const noexcept;
```

Tests whether the node is an alias node.  

## **Return Value**

`true` if the node is an alias node, `false` otherwise.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/is_alias.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/is_alias.output"
    ```

## **See Also**

* [add_anchor_name](add_anchor_name.md)
* [alias_of](alias_of.md)
