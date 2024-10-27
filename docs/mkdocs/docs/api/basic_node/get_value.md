<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_value

```cpp
template <
    typename T, typename ValueType = detail::remove_cvref_t<T>,
    detail::enable_if_t<
        detail::conjunction<
            std::is_default_constructible<ValueType>, detail::has_from_node<basic_node, ValueType>>::value,
        int> = 0>
T get_value() const noexcept(
    noexcept(ConverterType<ValueType>::from_node(std::declval<const basic_node&>(), std::declval<ValueType&>())));
```

Explicit type conversion between the internally stored YAML node value and a compatible value which is [copy-constructible](https://en.cppreference.com/w/cpp/named_req/CopyConstructible) and [default-constructible](https://en.cppreference.com/w/cpp/named_req/DefaultConstructible).  
The conversion relies on the [`node_value_converter`](../node_value_converter/index.md)::[`from_node`](../node_value_converter/from_node.md).  
This API makes a copy of the value.  
If the copying costs a lot, or if you need an address of the original value, then you should call [`get_value_ref`](get_value_ref.md) instead.  

If the YAML node value is a null, boolean, integer or floating point, this function internally executes type conversion according to the following rules which all depend on the template paramter type `T`:
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

Note that those scalar type cannot be converted to a sequence, mapping, string scalar and throws a [`type_error`](../exception/type_error.md).

## **Template Parameters**

***T***
:   A compatible value type which might be cv-qualified or a reference type.  

***ValueType***
:   A compatible value type.  
    This is, by default, a type of [std::remove_cvref_t<T>](https://en.cppreference.com/w/cpp/types/remove_cvref).  
    If fkYAML is compiled with C++11, C++14 or C++17, fkYAML uses its own implementation.  

## **Return Value**

A compatible native data value converted from the basic_node object.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_get_value.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_get_value.output"
    ```

## **See Also**

* [basic_node](index.md)
* [get_value_ref](get_value_ref.md)
* [node_value_converter::from_node](../node_value_converter/from_node.md)
