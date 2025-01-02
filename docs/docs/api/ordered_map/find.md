<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/ordered_map.hpp)</small>

# <small>fkyaml::ordered_map::</small>find

```cpp
template <typename KeyType>
iterator find(KeyType&& key) noexcept;

template <typename KeyType>
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

A (constant) iterator to the target value if found, the result of end() otherwise.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/ordered_map/emplace.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/ordered_map/emplace.output"
    ```

## **See Also**

* [ordered_map](index.md)
* [node](../basic_node/node.md)
* [basic_node::serialize](../basic_node/serialize.md)
* [operator<<(basic_node)](../basic_node/insertion_operator.md)
