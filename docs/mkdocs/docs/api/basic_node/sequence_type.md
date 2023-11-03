<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>sequence_type

```cpp
using sequence_type = SequenceType<basic_node>;
```

The type used to store sequence node values.  

To store sequence objects in [`basic_node`](index.md) class, the type is defined by the template parameter `SequenceType` which chooses the type to use for sequence objects.  
If not explicitly specified, the default type `std::vector` will be chosen.  
Note that sequence objects are stored as pointers to the decided type in a [`basic_node`](index.md) so that the internal storage size will at most be 8 bytes.  

???+ Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <type_traits>
    #include <vector>
    #include <fkYAML/node.hpp>

    int main()
    {
        std::cout << std::boolalpha
                  << std::is_same<std::vector<fkyaml::node>, fkyaml::node::sequence_type>::value
                  << std::endl;
        return 0;
    }
    ```

    output:
    ```bash
    true
    ```

### **See Also**

* [basic_node](index.md)
