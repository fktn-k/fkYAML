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
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>& rhs) noexcept(noexcept(lhs.swap(rhs))); // (2) non-member function
```

Swaps the internally stored data with the given basic_node object.

## **Parameters**

***`lhs`*** [in, out]
:   A basic_node to swap the contents with.

***`rhs`*** [in, out]
:   A basic_node to swap the contents with.

??? Example "member function: Swap the contents with another basic_node"

    ```cpp
    --8<-- "apis/basic_node/swap_member.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/swap_member.output"
    ```

## **Examples**

??? Example "non-member function: Swap the contents between basic_node objects"

    ```cpp
    --8<-- "apis/basic_node/swap_std.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/swap_std.output"
    ```

## **See Also**

* [basic_node](index.md)
* [get_value](get_value.md)
* [get_value_ref](get_value_ref.md)
