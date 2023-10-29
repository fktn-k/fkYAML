<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>const_iterator

```cpp
using const_iterator = detail::iterator<const basic_node>;
```

The type for constant iterators of [`basic_node`](index.md) containers.  
This iterator type is commonly used for sequence and mapping container values.  

??? Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create YAML nodes.
        const fkyaml::node sequence_node = {1, 2, 3};
        // get an iterator to the first sequence element.
        fkyaml::node::const_iterator it = sequence_node.begin();
        std::cout << fkyaml::node::serialize(*it) << std::endl;
        return 0;
    };
    ```

    output:
    ```bash
    1
    ```

### **See Also**

* [basic_node](index.md)
* [begin](begin.md)
* [iterator](iterator.md)
