<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>(constructor)

```cpp
basic_node() = default; // (1)

explicit basic_node(const node_type type); // (2)

explicit basic_node(const node_t type); // (3) deprecated

basic_node(const basic_node& rhs); // (4)

basic_node(basic_node&& rhs) noexcept; // (5)

template <typename CompatibleType, typename U = detail::remove_cvref_t<CompatibleType>>
basic_node(CompatibleType&& val) noexcept(
    noexcept(ConverterType<U>::to_node(std::declval<basic_node&>(), std::declval<CompatibleType>()))); // (6)

basic_node(initializer_list_t init); // (7)
```

Constructs a new basic_node from a variety of data sources.  
Available overloads are:

1. Constructs a basic_node with a null value.  
   The resulting basic_node has the [`node_type::NULL_OBJECT`](../node_type.md) type.
2. Constructs a basic_node with the given type.  
   The resulting basic_node has a default value for the given type.  

    | `type`                | default value    |
    | --------------------- | ---------------- |
    | node_type::SEQUENCE   | (empty sequence) |
    | node_type::MAPPING    | (empty mapping)  |
    | node_type::NULL_VALUE | `#!cpp nullptr`  |
    | node_type::BOOLEAN    | `#!cpp false`    |
    | node_type::INTEGER    | `#!cpp 0`        |
    | node_type::FLOAT      | `#!cpp 0.0`      |
    | node_type::STRING     | (empty string)   |

3. Constructs a basic_node with the given type.  
   The resulting basic_node has a default value for the given type.  
   Default values are the same as those in the above table.  

    !!! warning "Deprecation"

        The overload(2) `#!cpp basic_node(const node_type);` replaces the overload(3) `#!cpp basic_node(const node_t);` which has been deprecated in version 0.3.12. It will be removed in a future version. Please replace calls like  

        ```cpp
        fkyaml::node n(fkyaml::node::node_t::MAPPING);
        ```

        with  

        ```cpp
        fkyaml::node n(fkyaml::node_type::MAPPING);
        ```

4. Copy constructs a basic_node with an existing basic_node.
5. Move constructs a basic_node with an existing basic_node.  
   The existing basic_node will be the same as a default-constructed basic_node at the end of this constructor.
6. Constructs a basic_node with a value of a compatible type (see below).  
   The resulting basic_node has the value of `val` and the type which is associated with `CompatibleType`.  
   Template parameter `CompatibleType` includes, but not limited to, the following types:  
    * sequences:
        * [`sequence_type`](sequence_type.md)
        * container types which fulfills the following requirements, e.g., `std::vector` or `std::set`.
            * Both `begin()` and `end()` are callable on a `CompatibleType` object.
            * `CompatibleType` doesn't have both `key_type` and `mapped_type` member types.
            * A [`string_type`](string_type.md) object is not constructible from a `CompatibleType` object.
        * `std::pair` and `std::tuple`
    * mappings
        * [`mapping_type`](mapping_type.md)
        * container types which fullfills the following requirements, e.g., `std::map` or `std::unordered_multimap`.
            * Both `begin()` and `end()` are callable on a `CompatibleType` object.
            * `CompatibleType` has both `key_type` and `mapped_type` member types.
    * null
        * [`std::nullptr_t`](https://en.cppreference.com/w/cpp/types/nullptr_t)
    * booleans
        * [`boolean_type`](boolean_type.md)
    * integers
        * [`integer_type`](integer_type.md)
        * all integral types except `bool` such as `int`, `uint32_t` or `size_t`.
    * floating point values
        * [`float_number_type`](float_number_type.md)
        * `float`, `double` or `long double`
    * strings
        * [`string_type`](string_type.md)
        * types with which [`string_type`](string_type.md) is constructible such as `char[]`, `char*` or [`std::string_view`](https://en.cppreference.com/w/cpp/string/basic_string_view) (since C++17)

    You can add types to meet your needs by implementing custom `to_node()` functions. See [`node_value_converter`](../node_value_converter/to_node.md) for details.
7. Constructs a basic_node with a initializer_list_t object.  
   The resulting basic_node has the value of a container (sequence or mapping) which has the contents of `init`.  
   Basically, `init` is considered to be a sequence.  
   If `init` contains a sequence where each element contains 2 basic_node objects, however, such a sequence is interpreted as a mapping.  

    !!! Note "initializer lists which have a single element"

        `#!cpp fkyaml::node n {true};` will create a sequence of 1 boolean value, not a boolean value alone. This is because [overload resolution in C++ prioritizes a constructor with a single initializer list argument is prioritized to the others](https://en.cppreference.com/w/cpp/language/overload_resolution). In such cases, change the code to `#! fkyaml::node n(true);` instead.

## **Template Parameters**

***CompatibleType***
:   A type such that `ConverterType<U>` has a `to_node` function.

***U***
:   A type referred to by `ValueType` with its topmost cv-qualifiers removed.

## **Parameters**

***`type`*** [in]
:   the type of the value to create

***`rhs`*** [in]
:   the basic_node object to copy/move the value from

***`val`*** [in]
:   the value of a compatible type.

***`init`*** [in]
:   initializer list of basic_node objects

## **Examples**

??? Example "Overload(1): create a default value"

    ```cpp
    --8<-- "apis/basic_node/constructor_1.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/constructor_1.output"
    ```

??? Example "Overload(2): create an empty value with a given type"

    ```cpp
    --8<-- "apis/basic_node/constructor_2.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/constructor_2.output"
    ```

??? Example "Overload(3): create an empty value with a given type (deprecated)"

    ```cpp
    --8<-- "apis/basic_node/constructor_3.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/constructor_3.output"
    ```

??? Example "Overload(4): copy construct a basic_node object"

    ```cpp
    --8<-- "apis/basic_node/constructor_4.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/constructor_4.output"
    ```

??? Example "Overload(5): move construct a basic_node object"

    ```cpp
    --8<-- "apis/basic_node/constructor_5.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/constructor_5.output"
    ```

??? Example "Overload(6): create a basic_node object with compatible types"

    ```cpp
    --8<-- "apis/basic_node/constructor_6.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/constructor_6.output"
    ```

??? Example "Overload(7): create a basic_node object from an initializer list"

    ```cpp
    --8<-- "apis/basic_node/constructor_7.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/constructor_7.output"
    ```
---

## **See Also**

* [basic_node](index.md)
* [operator<<](insertion_operator.md)
