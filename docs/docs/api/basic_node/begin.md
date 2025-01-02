<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>begin, <small>fkyaml::basic_node::</small>cbegin

```cpp
iterator begin();
const_iterator begin() const;
const_iterator cbegin() const;
```

Returns a (constant) iterator to the first element of a container node.  
If a basic_node is neither a sequence nor mapping, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

![Image from https://en.cppreference.com/w/cpp/iterator/begin](../../img/range-begin-end.svg)

## **Return Value**

A (constant) iterator to the first element of a container node.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/begin.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/begin.output"
    ```

### **See Also**

* [basic_node](index.md)
* [node](node.md)
* [iterator, const_iterator](iterator.md)  
* [end, cend](end.md)
* [operator<<](insertion_operator.md)
