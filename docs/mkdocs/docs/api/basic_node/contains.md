<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>contains

```cpp
template <
    typename KeyType, detail::enable_if_t<
                            detail::is_usable_as_key_type<
                                typename mapping_type::key_compare, typename mapping_type::key_type, KeyType>::value,
                            int> = 0>
bool contains(KeyType&& key) const;
```
