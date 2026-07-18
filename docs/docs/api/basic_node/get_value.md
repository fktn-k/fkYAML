<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_value

```cpp
template <typename T, typename ValueType = detail::remove_cvref_t<T>>
T get_value() const noexcept(
    noexcept(ConverterType<ValueType>::from_node(std::declval<const basic_node&>(), std::declval<ValueType&>()))); // (1)

template <typename BasicNodeType>
BasicNodeType get_value() const; // (2)
```

This function converts a [`fkyaml::basic_node`](./index.md) to either  

1. a compatible value ([copy-constructible](https://en.cppreference.com/w/cpp/named_req/CopyConstructible) and [default-constructible](https://en.cppreference.com/w/cpp/named_req/DefaultConstructible))  
   The function is equivalent to executing  
   ```cpp
   ValueType ret;
   ConverterType<ValueType>::from_node(*this, ret);
   return ret;
   ```
   This library implements conversions from a node to a number of STL container types and scalar types. (see the notes down below)  
   Note that ValueType cannot be either a reference, pointer or C-style array type except `std::nullptr_t` since safe conversion is impossible with such types.  
   If you want to convert a node into a C-style array, consider using the [`get_value_inplace`](./get_value_inplace.md) function instead.
2. a [fkyaml::basic_node](./index.md) object  
   The function is equivalent to executing  
   ```cpp
   return *this; // Copy-constructs a new basic_node object.
   ```

Actual conversions rely on the [`node_value_converter`](../node_value_converter/index.md)::[`from_node`](../node_value_converter/from_node.md) function.  
This API makes a copy of the value, and if the copying costs too much, or if you need an address of the original value, then you should call one of the following functions instead.
* [`as_seq`](as_seq.md)
* [`as_map`](as_map.md)
* [`as_bool`](as_bool.md)
* [`as_int`](as_int.md)
* [`as_float`](as_float.md)
* [`as_str`](as_str.md)

??? Note "Convert from a Sequence Node"

    This library implements conversions from a sequence node to a number of STL container types whose element type is not a key-value pair. The implementation can be used for custom container types, but they need to have both `iterator` member type and `insert()` member function. The test suite confirms successful conversions to the following types.
    
    * *Sequence Containers*
        * [std::array](https://en.cppreference.com/w/cpp/container/array)
        * [std::vector](https://en.cppreference.com/w/cpp/container/vector)
        * [std::deque](https://en.cppreference.com/w/cpp/container/deque)
        * [std::forward_list](https://en.cppreference.com/w/cpp/container/forward_list)
        * [std::list](https://en.cppreference.com/w/cpp/container/list)
    * *Associative Containers for Keys*
        * [std::set](https://en.cppreference.com/w/cpp/container/set)
        * [std::multiset](https://en.cppreference.com/w/cpp/container/multiset)
    * *Unordered Associative Containers for Keys*
        * [std::unordered_set](https://en.cppreference.com/w/cpp/container/unordered_set)
        * [std::unordered_multiset](https://en.cppreference.com/w/cpp/container/unordered_multiset)
    * *Container Adapters*
        * [std::stack](https://en.cppreference.com/w/cpp/container/stack)
        * [std::queue](https://en.cppreference.com/w/cpp/container/queue)
        * [std::priority_queue](https://en.cppreference.com/w/cpp/container/priority_queue)
    * *Others*
        * [std::valarray](https://en.cppreference.com/w/cpp/numeric/valarray)
        * [std::pair](https://en.cppreference.com/w/cpp/utility/pair)
        * [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple)

    Note that the above types cannot be converted from a non-sequence node, which results in throwing a [type_error](../exception/type_error.md).

??? Note "Convert from a Mapping Node"

    This library implements conversions from a mapping node to STL container types whose element type is a key-value pair. The implementation can be used for custom container types, but they need to have `key_type`, `mapped_type` and `value_type` member types and `emplace()` member function. The test suite confirms successful conversions to the following types.

    * *Associative Containers for Key-Value Pairs*
        * [std::map](https://en.cppreference.com/w/cpp/container/map)
        * [std::multimap](https://en.cppreference.com/w/cpp/container/multimap)
    * *Unordered Associative Containers for Key-Value Pairs*
        * [std::unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map)
        * [std::unordered_multi_map](https://en.cppreference.com/w/cpp/container/unordered_multimap) *(unordered associative containers for key-value pairs)*

??? Note "Convert from a Null or Numeric Scalar Node"

    If the YAML node value is a null, boolean, integer or floating point, this function internally executes type conversion according to the following rules which all depend on the template parameter type `T`:

    * If the YAML node value is a **null** (node_type::NULL_OBJECT), the value can be converted to:
        * `false` (boolean)
        * `0` (integer)
        * `0.0` (floating point)
    * If the YAML node value is a **boolean** (node_type::BOOLEAN), the value can be converted to:
        * `1 /*true*/` or `0 /*false*/` (integer)
        * `1.0 /*true*/` or `0.0 /*false*/` (floating point)
    * If the YAML node value is a **integer** (node_type::INTEGER), the value can be converted to:
        * `true /*non-0*/` or `false /*0*/` (boolean)
        * `static_cast`ed floating point value (floating point)
    * If the YAML node value is a **floating point** (node_type::FLOAT), the value can be converted to:
        * `true /*non-0*/` or `false /*0*/` (boolean)
        * `static_cast`ed integer value (integer)

    Note that those scalar type cannot be converted to a sequence, mapping, string scalar, which results in throwing a [`type_error`](../exception/type_error.md).

??? Note "Convert from a String Scalar Node"

    String scalar nodes can be converted to STL container types which can be constructible from `const fkyaml::basic_node::string_type&` (`const std::string&` by default). The test suite confirms successful conversions to the following types.

    * [std::string](https://en.cppreference.com/w/cpp/string/basic_string)
    * [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view) (from C++17)

??? Note "Convert as an optional value"

    Since C++17, [std::optional](https://en.cppreference.com/w/cpp/utility/optional) can be used as `ValueType` to indicate the conversion result to be optional. In such cases, a returned [std::optional](https://en.cppreference.com/w/cpp/utility/optional) value contains a value only if the conversion was successful, or [`std::nullopt`](https://en.cppreference.com/w/cpp/utility/optional/nullopt) otherwise.

## **Template Parameters**

***T***
:   A compatible value type which might be cv-qualified or a reference type.  

***ValueType***
:   A compatible value type.  
    This is, by default, a type of [std::remove_cvref_t<T>](https://en.cppreference.com/w/cpp/types/remove_cvref).  
    If fkYAML is compiled with C++11, C++14 or C++17, fkYAML uses its own implementation.  

***BasicNodeType***
:   A basic_node template instance type.  

## **Return Value**

A compatible native data value converted from the [basic_node](./index.md) object.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/get_value.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/get_value.output"
    ```

## **See Also**

* [basic_node](index.md)
* [get_value_inplace](get_value_inplace.md)
* [as_seq](as_seq.md)
* [as_map](as_map.md)
* [as_bool](as_bool.md)
* [as_int](as_int.md)
* [as_float](as_float.md)
* [as_str](as_str.md)
* [node_value_converter::from_node](../node_value_converter/from_node.md)
