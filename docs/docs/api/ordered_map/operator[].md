<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/ordered_map.hpp)</small>

# <small>fkyaml::ordered_map::</small>operator[]

```cpp
template <typename KeyType>
mapped_type& operator[](KeyType&& key) noexcept;
```

Accesses an element with the given key.  

!!! Tip

    This API behaves like the [`std::map`](https://en.cppreference.com/w/cpp/container/map) class does.  
    In other words, it does not check the existence of the given key, and could create and return a default value if the key does not exist.  
    To avoid such a behavior, you can use [`at()`](at.md) function which throws a [`fkyaml::exception`](../exception/index.md) if the given key does not exist in the ordered_map object.

## **Template Parameters**

***KeyType***
:   A type compatible with the key type.

## **Parameters**

***`key`*** [in]
:   A key to the target value.

## **Return Value**

Reference to a `mapped_type` object associated with the given key.  
Possibly a default value of the `mapped_type` if the ordered_map does not contain the given key.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/ordered_map/subscript_operator.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/ordered_map/subscript_operator.output"
    ```

## **See Also**

* [ordered_map](index.md)
* [at](at.md)
* [node](../basic_node/node.md)
* [basic_node::serialize](../basic_node/serialize.md)
* [operator<<(basic_node)](../basic_node/insertion_operator.md)
