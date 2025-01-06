<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>begin, <small>fkyaml::basic_node::</small>cbegin

```cpp
map_range map_items();
const_map_range map_items() const;
```

Returns a (constant) range of mapping entries.  
If a basic_node is not a mapping, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

This function allows accessing the `key()` and `value()` functions provided by `iterator` and `const_iterator` in range-based for loops:  
```cpp
// without map_items()
for (auto it : mapping) {
    // `it` is of type fkyaml::node::reference and has no key() member function.
    std::cout << "value: " << it << std::endl;
}

// with map_items()
for (auto it : mapping.map_items()) {
    // `it` is now of type fkyaml::node::iterator and has key() member function.
    std::cout << "key: " << it.key() << ", value: " << it.value() << std::endl;
}
```

Also, this function allows using [structured bindings](https://en.cppreference.com/w/cpp/language/structured_binding) (since C++17):  
```cpp
for (auto& [key, value] : mapping.map_items()) {
    // `key` and `value` are both of type fkyaml::node::reference.
    std::cout << "key: " << key << ", value: " << value << std::endl;
}
```

## **Return Value**

A (constant) iterator to the first element of a container node.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/map_items.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/map_items.output"
    ```

### **See Also**

* [basic_node](index.md)
* [node](node.md)
* [iterator, const_iterator](iterator.md)  
* [end, cend](end.md)
* [operator<<](insertion_operator.md)
