<small>Defined in header [`<fkYAML/node_value_converter.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node_value_converter.hpp)</small>

# <small>fkyaml::basic_node::</small>from_node

```cpp
template <typename BasicNodeType, typename TargetType = ValueType>
static auto to_node(BasicNodeType& n, TargetType&& val) noexcept(
    noexcept(::fkyaml::to_node(n, std::forward<TargetType>(val))))
    -> decltype(::fkyaml::to_node(n, std::forward<TargetType>(val)))
```
