<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_alias

```cpp
bool is_alias() const noexcept;
```

Tests whether the node is an alias node.  

### **Return Value**

`true` if the node is an alias node, `false` otherwise.  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_is_alias.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_is_alias.output"
    ```

### **See Also**

* [add_anchor_name](add_anchor_name.md)
* [alias_of](alias_of.md)
