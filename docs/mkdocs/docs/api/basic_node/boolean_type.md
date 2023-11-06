<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>boolean_type

```cpp
using boolean_type = BooleanType;
```

The type used to store boolean node values.  
The YAML specification describes a boolean as a type which differentiates the following literals:

* true, True, TRUE -> `true`
* false, False, FALSE -> `false`

To store boolean objects in [`basic_node`](index.md) class, the type is defined by the template parameter `BooleanType` which chooses the type to use for boolean objects.  
If not explicitly specified, the default type `bool` will be chosen.  
With the decided type, boolean objects are stored directly inside a [`basic_node`](index.md).  

???+ Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <type_traits>
    #include <fkYAML/node.hpp>

    int main()
    {
        std::cout << std::boolalpha << std::is_same<bool, fkyaml::node::boolean_type>::value << std::endl;
        return 0;
    }
    ```

    output:
    ```bash
    true
    ```

### **See Also**

* [basic_node](index.md)