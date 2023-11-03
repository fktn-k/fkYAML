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
If the copying costs a lot, or if you need an address of the original value, then it might be more suitable to call [`get_value_ref`](get_value_ref.md) instead.  

## **Template Parameters**

***T***
:   A compatible value type which might be cv-qualified or a reference type.  

***ValueType***
:   A compatible value type.  
    This is, by default, a result of [std::remove_cvref_t<T>](https://en.cppreference.com/w/cpp/types/remove_cvref).  

## **Return Value**

A compatible native data value converted from the basic_node object.
