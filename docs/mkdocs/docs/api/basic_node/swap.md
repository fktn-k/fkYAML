<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>swap

```cpp
void swap(basic_node& rhs) noexcept; // (1)

template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename = void> class ConverterType>
inline void swap(
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>& lhs,
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>& rhs) noexcept(noexcept(lhs.swap(rhs))); // (2)
```

Swaps the internally stored data with the given basic_node object.

## Overload (1)

```cpp
void swap(basic_node& rhs) noexcept; // (1)
```

### **Parameters**

***`rhs`*** [in]
:   A basic_node object to be swapped with.

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create YAML nodes.
        fkyaml::node n1 = 123;
        fkyaml::node n2 = "foo";

        // swap the internally stored data between n1 & n2.
        n1.swap(n2);

        // print the swapped values.
        std::cout << n1.get_value_ref<std::string&>() << std::endl;
        std::cout << n2.get_value<std::int64_t>() << std::endl;
        return 0;
    }
    ```

    output:
    ```bash
    foo
    123
    ```

## Overload (2)

```cpp
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename = void> class ConverterType>
inline void swap(
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>& lhs,
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>& rhs) noexcept(noexcept(lhs.swap(rhs))); // (2)
```

### **Parameters**

***`lhs`*** [in]
:   A left-hand-side basic_node object to be swapped with.

***`rhs`***
:   A right-hand-side basic_node object to be swapped with.

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create YAML nodes.
        fkyaml::node n1 = 123;
        fkyaml::node n2 = "foo";

        // swap the internally stored data between n1 & n2.
        using std::swap;
        swap(n1, n2);

        // print the swapped values.
        std::cout << n1.get_value_ref<std::string&>() << std::endl;
        std::cout << n2.get_value<std::int64_t>() << std::endl;
        return 0;
    }
    ```

    output:
    ```bash
    foo
    123
    ```

## **See Also**

* [basic_node](index.md)
* [get_value](get_value.md)
* [get_value_ref](get_value_ref.md)
