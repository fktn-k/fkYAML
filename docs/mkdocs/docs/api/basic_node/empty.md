<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>empty

```cpp
boolean empty() const;
```

Tests whether the node value is empty.  
Throws a [`fkyaml::exception`](../exception/index.md) if a basic_node does not have a conatiner nor string value.  

### **Return Value**

`true` if the node value is empty, `false` otherwise.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_empty.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_empty.output"
    ```

### **See Also**

* [basic_node](index.md)
