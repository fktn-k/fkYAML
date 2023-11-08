<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>iterator

```cpp
using iterator = detail::iterator<basic_node>;
```

The type for iterators of [`basic_node`](index.md) containers.  
This iterator type is commonly used for sequence and mapping container values.  

???+ Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create YAML nodes.
        fkyaml::node sequence_node = {1, 2, 3};
        // get an iterator to the first sequence element.
        fkyaml::node::iterator it = sequence_node.begin();
        std::cout << *it << std::endl;
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
* [const_iterator](const_iterator.md)
* [operator<<](insertion_operator.md)
