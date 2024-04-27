<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>begin

```cpp
iterator begin();
const_iterator begin() const;
```

Returns an iterator to the first element of a container node value.  
Throws a [`fkyaml::exception`](../exception/index.md) if a basic_node does not have a sequence nor mapping value.  

![Image from https://en.cppreference.com/w/cpp/iterator/begin](../../img/range-begin-end.svg)

### **Return Value**

An iterator to the first element of a container node value (either sequence or mapping).

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_begin.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_begin.output"
    ```

### **See Also**

* [basic_node](index.md)
* [node](node.md)
* [iterator](iterator.md)  
* [const_iterator](const_iterator.md)
* [end](end.md)
