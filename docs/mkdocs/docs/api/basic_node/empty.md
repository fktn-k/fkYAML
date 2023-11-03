<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>empty

```cpp
boolean empty() const;
```

Tests whether the node value is empty.  
Throws a [`fkyaml::exception`](../exception/index.md) if a basic_node does not have a conatiner nor string value.  

### **Return Value**

`true` if the node value is empty, `false` otherwise.

???+ Example

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
            {{"foo", true}, {"bar", false}},
            fkyaml::node(),
            true,
            256,
            3.14,
            "Hello, world!"
        };

        for (const auto& n : nodes)
        {
            try
            {
                // call empty()
                std::cout << std::boolalpha << n.empty() << std::endl;
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
    true
    true
    The node does not have a container nor string value.
    The node does not have a container nor string value.
    The node does not have a container nor string value.
    The node does not have a container nor string value.
    true
    ```

### **See Also**

* [basic_node](index.md)
