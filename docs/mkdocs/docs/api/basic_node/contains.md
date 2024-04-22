<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>contains

```cpp
template <
    typename KeyType, detail::enable_if_t<
                            detail::conjunction<
                                detail::negation<detail::is_basic_node<detail::remove_cvref_t<KeyType>>>,
                                detail::is_node_compatible_type<basic_node, detail::remove_cvref_t<KeyType>>>::value,
                            int> = 0>
bool contains(KeyType&& key) const; // (1) A compatible key object with basic_node type

template <
    typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
bool contains(KeyType&& key) const; // (2) A basic_node object
```

Checks if the YAML node has the given key.  
If the node value is not a mapping, this API will throw an [`fkyaml::type_error`](../exception/type_error.md).  
The `KeyType` can be a compatible type with [`fkyaml::basic_node`](index.md) or a kind of [`fkyaml::basic_node`](index.md) template class.

## **Template Parameters**

***KeyType***
:   A type which is compatible with or a kind of [`fkyaml::basic_node`](index.md).

## **Parameters**

***`key`*** [in]
:   A key to the target value in the YAML mapping node value.

## **Return Value**

`true` if the YAML node is a mapping and has the given key, `false` otherwise.  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_contains.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_contains.output"
    ```

## **See Also**

* [basic_node](index.md)
* [mapping_type](mapping_type.md)
* [type_error](../exception/type_error.md)
