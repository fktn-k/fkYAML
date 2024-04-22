<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_mapping

```cpp
bool is_mapping() const noexcept;
```

Tests whether the node value type is [`node_t::MAPPING`](node_t.md).  

### **Return Value**

`true` if the type is [`node_t::MAPPING`](node_t.md), `false` otherwise.  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_is_mapping.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_is_mapping.output"
    ```

### **See Also**

* [node_t](node_t.md)
