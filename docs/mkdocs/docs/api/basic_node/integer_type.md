<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>integer_type

```cpp
using integer_type = IntegerType;
```

The type used to store boolean node values.  

To store integer objects in [`basic_node`](index.md) class, the type is defined by the template parameter `IntegerType` which chooses the type to use for integer objects.  
If not explicitly specified, the default type `std::int64_t` will be chosen.  
With the decided type, integer objects are stored directly inside a [`basic_node`](index.md).  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_integer_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_integer_type.output"
    ```

### **See Also**

* [basic_node](index.md)
