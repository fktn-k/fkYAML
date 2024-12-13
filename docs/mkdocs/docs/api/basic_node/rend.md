<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>rend, <small>fkyaml::basic_node::</small>crend

```cpp
reverse_iterator rend();
const_reverse_iterator rend() const;
const_reverse_iterator crend() const;
```

Returns an iterator to the reverse-end (= one before the first) element of a container node.  
Throws a [`fkyaml::type_error`](../exception/type_error.md) if a basic_node is neither a sequence nor mapping node.  

![Image from https://en.cppreference.com/w/cpp/iterator/reverse_iterator](../../img/range-rbegin-rend.svg)

### **Return Value**

A (constant) reverse iterator to the reverse-end (= one before the first) element of a container node.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_rend.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_rend.output"
    ```

### **See Also**

* [basic_node](index.md)
* [node](node.md)
* [reverse_iterator, const_reverse_iterator](reverse_iterator.md)  
* [rend, crend](rend.md)