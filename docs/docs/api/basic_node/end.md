<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>end, <small>fkyaml::basic_node::</small>cend

```cpp
iterator end();
const_iterator end() const;
const_iterator cend() const;
```

Returns a (constant) iterator to the past-the-last element of a container node.  
If a basic_node is neither a sequence nor mapping, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

![Image from https://en.cppreference.com/w/cpp/iterator/end](../../img/range-begin-end.svg)

## **Return Value**

A (constant) iterator to the past-the-last element of a container node.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/end.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/end.output"
    ```

## **See Also**

* [basic_node](index.md)
* [node](node.md)
* [iterator, const_iterator](iterator.md)  
* [begin, cbegin](begin.md)
* [operator<<](insertion_operator.md)
