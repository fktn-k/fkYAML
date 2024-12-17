<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>size

```cpp
std::size_t size() const;
```

Gets the size of a node value.  
Throws a [`fkyaml::exception`](../exception/index.md) if a basic_node is neither a sequence, mapping nor string value.  

### **Return Value**

The size of a node value.

??? Example

    ```cpp
    --8<-- "examples/ex_basic_node_size.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_size.output"
    ```

### **See Also**

* [basic_node](index.md)
