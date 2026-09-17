<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>erase

```cpp
template <typename KeyType>
size_type erase(KeyType&& key);
```

Erases the entry with the given key from a YAML mapping node.  
If the node value is not a mapping, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

The input parameter `key` must be either a [`basic_node`](index.md) object or an object of a compatible type, i.e., a type from which a [`basic_node`](index.md) object can be constructed.  

## **Template Parameters**

***KeyType***
:   A type from which a [`basic_node`](index.md) object can be constructed.

## **Parameters**

***`key`*** [in]
:   A key identifying the mapping entry to erase.

## **Return Value**

The number of erased entries (`0` or `1`).

??? Example

    ```cpp
    --8<-- "apis/basic_node/erase.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/erase.output"
    ```

## **See Also**

* [basic_node](index.md)
* [contains](contains.md)
* [mapping_type](mapping_type.md)
* [type_error](../exception/type_error.md)
