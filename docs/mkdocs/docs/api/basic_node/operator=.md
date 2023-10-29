<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator=

```cpp
basic_node& operator=(const basic_node& rhs) noexcept; // (1)

basic_node& operator=(basic_node&& rhs) noexcept; // (2)
```

Assignment operator.  
The `basic_node` class supports both copy and move assignments.  

## Overload (1)

```cpp
basic_node& operator=(const basic_node& rhs) noexcept;
```

Copy assignment operator.  
Copies a YAML node value via the "copy and swap" strategy to enhance exception safety.  

??? Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        fkyaml::node n = true;
        fkyaml::node n2 = 123;
        n = n2;

        std::cout << std::boolalpha << n.is_integer() << std::endl;
        std::cout << n.get_value<std::int64_t>() << std::endl;

        return 0;
    }
    ```

    output:
    ```bash
    true
    123
    ```

## **See Also**

* [basic_node](index.md)
* [is_integer](is_integer.md)
* [get_value](get_value.md)
