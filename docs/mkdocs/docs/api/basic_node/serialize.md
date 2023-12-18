<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>serialize

```cpp
static std::string serialize(const basic_node& node);
```

Serializes YAML node values recursively.  
Currently, the serialization result only supports block styles.  
That means that, even if a deserialized source input is written in flow styles, the serialization result forces to convert it in block styles.  

### **Parameters**

***`node`*** [in]
:   A `basic_node` object to be serialized.

### **Return Value**

The resulting string object from the serialization of the `node` object.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_serialize.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_serialize.output"
    ```

### **See Also**

* [basic_node](index.md)
* [deserialize](deserialize.md)
