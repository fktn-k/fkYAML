<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/ordered_map.hpp)</small>

# <small>fkyaml::ordered_map::</small>at

```cpp
template <
    typename KeyType,
    detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
mapped_type& at(KeyType&& key) noexcept;

template <
    typename KeyType,
    detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
const mapped_type& at(KeyType&& key) const
```

Accesses an element with the given key.  
This API throws a [`fkyaml::exception`] if the given key does not exist in the ordered_map object.  

## **Template Parameters**

***KeyType***
:   A type compatible with the key type.

## **Parameters**

***`key`*** [in]
:   A key to the target value.

## **Return Value**

Reference, or constant reference, to a `mapped_type` object associated with the given key.  

???+ Example

    ```cpp
    --8<-- "examples/ex_ordered_map_at.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_ordered_map_at.output"
    ```

## **See Also**

* [ordered_map](index.md)
* [operator[]](operator[].md)
* [node](../basic_node/node.md)
* [basic_node::serialize](../basic_node/serialize.md)
* [operator<<(basic_node)](../basic_node/insertion_operator.md)
* [exception::what](../exception/what.md)
