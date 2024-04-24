<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>iterator

```cpp
using iterator = detail::iterator<basic_node>;
```

The type for iterators of [`basic_node`](index.md) containers.  
This iterator type is commonly used for sequence and mapping container values.  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_iterator.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_iterator.output"
    ```

### **See Also**

* [basic_node](index.md)
* [begin](begin.md)
* [const_iterator](const_iterator.md)
* [operator<<](insertion_operator.md)
