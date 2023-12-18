<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>string_type

```cpp
using string_type = StringType;
```

The type used to store integer node values and keys for mapping nodes.  

To store string objects in [`basic_node`](index.md) class, the type is defined by the template parameter `StringType` which chooses the type to use for string objects.  
If not explicitly specified, the default type `std::string` will be chosen.  
Note that string objects are stored as pointers to the decided type in a [`basic_node`](index.md) so that the internal storage size will at most be 8 bytes.  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_string_type.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_string_type.output"
    ```

### **See Also**

* [basic_node](index.md)
