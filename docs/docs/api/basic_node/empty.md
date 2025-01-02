<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>empty

```cpp
boolean empty() const;
```

Tests whether the node value is empty.  
If a basic_node is neither a sequence, mapping nor string, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

## **Return Value**

`true` if the node value is empty, `false` otherwise.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/empty.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/empty.output"
    ```

### **See Also**

* [basic_node](index.md)
