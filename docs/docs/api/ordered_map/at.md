<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/ordered_map.hpp)</small>

# <small>fkyaml::ordered_map::</small>at

```cpp
template <typename KeyType>
mapped_type& at(KeyType&& key) noexcept;

template <typename KeyType>
const mapped_type& at(KeyType&& key) const
```

Accesses an element with the given key.  
This function throws a [`fkyaml::exception`] if the given key does not exist in the ordered_map object.  

## **Template Parameters**

***KeyType***
:   A type compatible with the key type.

## **Parameters**

***`key`*** [in]
:   A key to the target value.

## **Return Value**

(Constant) reference to a `mapped_type` object associated with the given key.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/ordered_map/at.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/ordered_map/at.output"
    ```

## **See Also**

* [ordered_map](index.md)
* [operator[]](operator[].md)
* [node](../basic_node/node.md)
* [basic_node::serialize](../basic_node/serialize.md)
* [operator<<(basic_node)](../basic_node/insertion_operator.md)
* [exception::what](../exception/what.md)
