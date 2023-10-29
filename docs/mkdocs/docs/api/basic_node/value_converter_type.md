<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>value_converter_type

```cpp
template <typename T, typename SFINAE>
using value_converter_type = ValueConverterType<T, SFINAE>;
```

A helper type alias to determine converter type for the given target native data type `T`.  
The default type of the template parameter `ValueConverterType` is [`node_value_converter`](../node_value_converter/index.md).  
This type alias is used to determine the target type `T` is convertible from/to [`basic_node`](index.md).  
The function [`get_value`](get_value.md) relies on it internally.  
If you want to convert some type from/to `basic_node`, however, it is recommended to use [`basic_node::get_value`](get_value.md) instead, in order both to make your program more understandable and to avoid too much dependency on the fkYAML library implementation.  

### **Template Parameters**

***T***
: The target native data type.

***SFINAE***
: Type to add compile type checks via SFINAE. Usually `void` is given.

??? Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        std::cout << std::boolalpha
                  << std::is_same<
                         fkyaml::node_value_converter<double>,
                         fkyaml::node::value_converter_type<double>>::value
                  << std::endl;

        fkyaml::node n = 3.14;
        double d = 0.0;
        // This leads to the same result as `d = n.get_value<double>()`.
        fkyaml::value_converter_type<double, void>::from_node(n, d);
        std::cout << std::setprecision(3) << d << std::endl;

        return 0;
    }
    ```

    output:
    ```bash
    true
    3.14
    ```
