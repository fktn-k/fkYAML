<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>const_iterator

```cpp
using const_iterator = detail::iterator<const basic_node>;
```

The type for constant iterators of [`basic_node`](index.md) containers.  
This iterator type is commonly used for sequence and mapping container values.  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_const_iterator.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_const_iterator.output"
    ```

### **See Also**

* [basic_node](index.md)
* [begin](begin.md)
* [iterator](iterator.md)
