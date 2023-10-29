<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>size

```cpp
std::size_t size() const;
```

Gets the size of a node value.  
Throws a [`fkyaml::exception`](../exception/index.md) if a basic_node does not have a conatiner nor string value.  

### **Return Value**

Returns the size of a node value.

??? Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create YAML nodes.
        std::vector<fkyaml::node> nodes =
        {
            {1, 2, 3},
            {{std::string("foo"), true}, {std::string("bar"), false}. {std::string("baz"), true}},
            fkyaml::node(),
            true,
            256,
            3.14,
            std::string("foo")
        };

        for (const auto& n : nodes)
        {
            try
            {
                // call size()
                std::cout << n.size() << std::endl;
            }
            catch (const fkyaml::exception& e)
            {
                std::cout << "The node does not have a container nor string value." << std::endl;
            }
        }

        return 0;
    }
    ```

    output:
    ```yaml
    3
    3
    The node does not have a container nor string value.
    The node does not have a container nor string value.
    The node does not have a container nor string value.
    The node does not have a container nor string value.
    3
    ```

### **See Also**

* [basic_node](index.md)
