<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/ordered_map.hpp)</small>

# <small>fkyaml::ordered_map::</small>find

```cpp
template <
    typename KeyType,
    detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
iterator find(KeyType&& key) noexcept;

template <
    typename KeyType,
    detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
const_iterator find(KeyType&& key) const noexcept;
```

Find a value with the given key.  

## **Template Parameters**

***KeyType***
:   A type compatible with the key type.

## **Parameters**

***`key`*** [in]
:   A key to the target value.

## **Return Value**

An iterator to the target value if found, the result of end() otherwise.  

???+ Example

    ```cpp
    --8<-- "examples/ex_ordered_map_emplace.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_ordered_map_emplace.output"
    ```

## **See Also**

* [ordered_map](index.md)
* [node](../basic_node/node.md)
* [basic_node::serialize](../basic_node/serialize.md)
* [operator<<(basic_node)](../basic_node/insertion_operator.md)
