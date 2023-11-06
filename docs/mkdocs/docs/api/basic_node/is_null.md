<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>is_null

```cpp
bool is_null() const noexcept;
```

Tests whether the node value type is [`node_t::NULL_OBJECT`](node_t.md).  

### **Return Value**

`true` if the type is [`node_t::NULL_OBJECT`](node_t.md), `false` otherwise.  

???+ Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        fkyaml::node n;
        std::cout << std::boolalpha << n.is_null() << std::endl;
        return 0;
    }
    ```

    output:
    ```bash
    true
    ```

### **See Also**

* [node_t](node_t.md)
