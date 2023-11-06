<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/ordered_map.hpp)</small>

# <small>fkyaml::ordered_map::</small>(constructor)

```cpp
ordered_map(); // (1)

ordered_map(std::initializer_list<value_type> init); // (2)
```

Constructs a new ordered_map.  
You can specify the initial value on constructing an ordered_map with an overloaded constructor.  

## Overload (1)

```cpp
ordered_map(); // (1)
```

Constructs an ordered_map object without an initial value.  
The content of a newly constructed ordered_map is an empty list of key-value pairs.  

???+ Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>
    #include <fkYAML/ordered_map.hpp>

    int main()
    {
        fkyaml::ordered_map<std::string, fkyaml::node> om;
        std::cout << std::boolalpha << om.empty() << std::endl;
        return 0;
    }
    ```

    output:
    ```bash
    true
    ```

## Overload (2)

```cpp
ordered_map(std::initializer_list<value_type> init); // (2)
```

Constructs a new ordered_map with an initializer list.  
The resulting ordered_map object has the same list of key-value pairs as the given initializer list.  

## **Parameters**

***`init`*** [in]
:   An initializer list of key-value pairs.

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>
    #include <fkYAML/ordered_map.hpp>

    int main()
    {
        fkyaml::ordered_map<std::string, fkyaml::node> om = {
            { "foo", 123 },
            { "bar", "baz" }
        };

        for (auto& pair : om)
        {
            std::cout << pair.first << ": " << fkyaml::node::serialize(pair.second) << std::endl;
        }
        return 0;
    }
    ```

    output:
    ```yaml
    foo: 123
    bar: baz
    ```

## **See Also**

* [ordered_map](index.md)
* [basic_node](../basic_node/index.md)
* [basic_node::(constructor)](../basic_node/constructor.md)
* [basic_node::serialize](../basic_node/serialize.md)
