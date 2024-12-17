<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_anchor

```cpp
bool is_anchor() const noexcept;
```

Tests whether the node is an anchor node.  

### **Return Value**

`true` if the node is an anchor node, `false` otherwise.  

??? Example

    ```cpp
    --8<-- "examples/ex_basic_node_is_anchor.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_is_anchor.output"
    ```

### **See Also**

* [add_anchor_name](add_anchor_name.md)
