<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>rbegin, <small>fkyaml::basic_node::</small>crbegin

```cpp
reverse_iterator rbegin();
const_reverse_iterator rbegin() const;
const_reverse_iterator crbegin() const;
```

Returns a (constant) iterator to the reverse-beginning (= last) element of a container node.  
Throws a [`fkyaml::type_error`](../exception/type_error.md) if a basic_node is neither a sequence nor mapping node.  

![Image from https://en.cppreference.com/w/cpp/iterator/reverse_iterator](../../img/range-rbegin-rend.svg)

## **Return Value**

A (constant) iterator to the reverse-beginning (= last) element of a container node.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/rbegin.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/rbegin.output"
    ```

## **See Also**

* [basic_node](index.md)
* [node](node.md)
* [reverse_iterator, const_reverse_iterator](reverse_iterator.md)  
* [rend, crend](rend.md)
