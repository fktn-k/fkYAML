<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/ordered_map.hpp)</small>

# <small>fkyaml::ordered_map::</small>emplace

```cpp
template <
    typename KeyType,
    detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
std::pair<iterator, bool> emplace(KeyType&& key, const mapped_type& value) noexcept;
```

Emplaces a new key-value pair if the new key does not exist in the ordered_map object.

***KeyType***
:   A type compatible with the key type.

## **Parameters**

***`key`*** [in]
:   A key to the target value.

***`value`*** [in]
:   A value associated to the key.

## **Return Value**

A pair consisting of an iterator to the inserted element, or the already-existing element if no insertion happend, and a boolean denoting the insertion took place (`true` if insertion happened, `false` otherwise).

???+ Example

    ```cpp
    --8<-- "examples/ex_ordered_map_constructor_initializer_list.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_ordered_map_constructor_initializer_list.output"
    ```

## **See Also**

* [ordered_map](index.md)
* [node](../basic_node/node.md)
* [basic_node::serialize](../basic_node/serialize.md)
* [operator<<(basic_node)](../basic_node/insertion_operator.md)
