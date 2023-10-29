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
