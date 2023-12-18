<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_scalar

```cpp
bool is_scalar() const noexcept;
```

Tests whether the node value type is one of the followings:  
* [`node_t::NULL_OBJECT`](node_t.md)
* [`node_t::BOOLEAN`](node_t.md)
* [`node_t::INTEGER`](node_t.md)
* [`node_t::FLOAT_NUMBER`](node_t.md)
* [`node_t::STRING`](node_t.md)

### **Return Value**

`true` if the type is a scalar type, `false` otherwise.  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_is_scalar.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_is_scalar.output"
    ```

### **See Also**

* [node_t](node_t.md)
