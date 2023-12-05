<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator==

```cpp
bool operator==(const basic_node& rhs) const noexcept;
```

Equal-to operator.  
Compares two `basic_node` objects for equality according to the following rules:  

* Two `basic_node` objects are equal if they are of the same [`node_t`](node_t.md) type and their stored values are the same according to their respective `operator==`.
* Two `basic_node` objects are always equal if both of them are of the [`node_t::NULL_OBJECT`](node_t.md) type.

## **Parameters**

***`rhs`*** [in]
:   A `basic_node` object to be compared with `this` object.

## **Return Value**

`true` if both types and values are equal, `false` otherwise.

???+ Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        fkyaml::node seq_1 = {1, 2, 3};
        fkyaml::node seq_2 = {1, 2, 4};
        fkyaml::node map_1 = {{"foo", true}, {"bar", 123}};
        fkyaml::node map_1 = {{"bar", 123}, {"foo", true}};
        fkyaml::node null_1 = nullptr;
        fkyaml::node null_2 = nullptr;
        fkyaml::node bool_1 = true;
        fkyaml::node bool_2 = false;
        fkyaml::node integer_1 = 123;
        fkyaml::node integer_2 = 123;
        fkyaml::node float_1 = 3.14;
        fkyaml::node float_2 = 3.14;
        fkyaml::node string_1 = "foo";
        fkyaml::node string_2 = "bar";

        std::cout << std::boolalpha;
        std::cout << seq_1 == seq_2 << std::endl;
        std::cout << map_1 == map_2 << std::endl;
        std::cout << null_1 == null_2 << std::endl;
        std::cout << bool_1 == bool_2 << std::endl;
        std::cout << integer_1 == integer_2 << std::endl;
        std::cout << float_1 == float_2 << std::endl;
        std::cout << string_1 == string_2 << std::endl;

        return 0;
    }
    ```

    output:
    ```bash
    false
    true
    true
    false
    true
    true
    false
    ```

## **See Also**

* [basic_node](index.md)
* [operator!=](operator_ne.md)
