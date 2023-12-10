<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator<=

```cpp
bool operator<=(const basic_node& rhs) const noexcept;
```

Less-than-or-equal-to operator.  
Check if `this` object is less than or equal to `rhs`.  
The operator returns the result of `!(rhs < *this)`. (see [`operator<`](operator_lt.md))  

## **Parameters**

***`rhs`*** [in]
:   A `basic_node` object to be compared with `this` object.

## **Return Value**

`true` if `this` is less than or equal to `rhs`, `false` otherwise.

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
        fkyaml::node bool_1 = false;
        fkyaml::node bool_2 = true;
        fkyaml::node integer_1 = 321;
        fkyaml::node integer_2 = 123;
        fkyaml::node float_1 = 1.23;
        fkyaml::node float_2 = 3.14;
        fkyaml::node string_1 = "foo";
        fkyaml::node string_2 = "bar";

        // the same type
        std::cout << std::boolalpha;
        std::cout << seq_1 <= seq_2 << std::endl;
        std::cout << map_1 <= map_2 << std::endl;
        std::cout << null_1 <= null_2 << std::endl;
        std::cout << bool_1 <= bool_2 << std::endl;
        std::cout << integer_1 <= integer_2 << std::endl;
        std::cout << float_1 <= float_2 << std::endl;
        std::cout << string_1 <= string_2 << std::endl;

        // different types
        std::cout << bool_1 <= string_1 << std::endl;
        std::cout << float_2 <= seq_2 << std::endl;

        return 0;
    }
    ```

    output:
    ```bash
    true
    true
    true
    true
    false
    true
    false
    true
    false
    ```

## **See Also**

* [basic_node](index.md)
* [operator==](operator_eq.md)
