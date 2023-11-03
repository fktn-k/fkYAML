<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>float_number_type

```cpp
using float_number_type = FloatNumberType;
```

The type used to store boolean node values.  
The YAML specification describes a floating point number as a type which differentiates the following literals:

* .inf, .Inf, .INF -> an infinite number
* .nan, .NaN, .NAN -> Not a Number

To store floating point number objects in [`basic_node`](index.md) class, the type is defined by the template parameter `FloatNumberType` which chooses the type to use for floating point number objects.  
If not explicitly specified, the default type `double` will be chosen.  
With the decided type, floating point number objects are stored directly inside a [`basic_node`](index.md).  

???+ Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <type_traits>
    #include <fkYAML/node.hpp>

    int main()
    {
        std::cout << std::boolalpha
                  << std::is_same<double, fkyaml::node::float_number_type>::value
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
