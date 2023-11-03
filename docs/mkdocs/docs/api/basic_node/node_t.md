<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>node_t

```cpp
enum class node_t
{
    SEQUENCE,
    MAPPING,
    NULL_OBJECT,
    BOOLEAN,
    INTEGER,
    FLOAT_NUMBER,
    STRING,
};
```

This enumeration collects the different YAML value types. They are internally used to distinguish the stored values, and the functions [`is_sequence`](is_sequence.md), [`is_mapping`](is_mapping.md) and [`is_scalar`](is_scalar.md) (with [`is_null`](is_null.md), [`is_boolean`](is_boolean.md), [`is_integer`](is_integer.md), [`is_float_number`](is_float_number.md), [`is_string`](is_string.md)) rely on it.  

!!! Note "Types of scalars"

    There are five enumerators for scalars (`NULL_OBJECT`, `BOOLEAN`, `INTEGER`, `FLOAT_NUMBER`, `STRING`) to distinguish between different types of scalars:  

    * [`boolean_type`](boolean_type.md) for boolean scalar values
    * [`integer_type`](integer_type.md) for integer scalar values
    * [`float_number_type`](float_number_type.md) for float number scalar values
    * [`string_type`](string_type.md) for string scalar values

???+ Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create YAML nodes.
        fkyaml::node sequence_node = {1, 2, 3};
        fkyaml::node mapping_node = {{std::string("foo"), true}, {std::string("bar"), false}};
        fkyaml::node null_node;
        fkyaml::node boolean_node = true;
        fkyaml::node integer_node = 256;
        fkyaml::node float_node = 3.14;
        fkyaml::node string_node = std::string("Hello, world!");

        // call type()
        std::cout << std::boolalpha;
        std::cout << (sequence_node.type() == fkyaml::node::node_t::SEQUENCE) << std::endl;
        std::cout << (mapping_node.type() == fkyaml::node::node_t::MAPPING) << std::endl;
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
    true
    true
    ```

### **See Also**

* [type](type.md)
* [is_sequence](is_sequence.md)
* [is_mapping](is_mapping.md)
* [is_scalar](is_scalar.md)
* [is_null](is_null.md)
* [is_boolean](is_boolean.md)
* [is_integer](is_integer.md)
* [is_float_number](is_float_number.md)
* [is_string](is_string.md)