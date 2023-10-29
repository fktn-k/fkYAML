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

??? Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create YAML nodes.
        fkyaml::node null_node;
        fkyaml::node boolean_node = true;
        fkyaml::node integer_node = 256;
        fkyaml::node float_node = 3.14;
        fkyaml::node string_node = std::string("Hello, world!");

        // call type()
        std::cout << std::boolalpha;
        std::cout << (null_node.type() == fkyaml::node::node_t::NULL_OBJECT) << std::endl;
        std::cout << (boolean_node.type() == fkyaml::node::node_t::BOOLEAN) << std::endl;
        std::cout << (integer_node.type() == fkyaml::node::node_t::INTEGER) << std::endl;
        std::cout << (float_node.type() == fkyaml::node::node_t::FLOAT_NUMBER) << std::endl;
        std::cout << (string_node.type() == fkyaml::node::node_t::STRING) << std::endl;

        return 0;
    };
    ```

    output:
    ```bash
    true
    true
    true
    true
    true
    ```

### **See Also**

* [node_t](node_t.md)
