<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>contains

```cpp
template <typename KeyType>
bool contains(KeyType&& key) const; // (1)

template <typename BasicNodeType>
bool contains(BasicNodeType&& key) const; // (2)
```

Checks if the YAML node has the given key in the queried mapping.  
If the node value is not a mapping, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

The input parameter `key` must be either a [`basic_node`](index.md) object or an object of a compatible type, i.e., a type with which a [`basic_node`](index.md) object can be constructible.  
Note that the overload (1) internally constructs a temporal [`basic_node`](index.md) object.  
So, if you use the same key multiple times, for example, in a for loop, consider creating a [`basic_node`](index.md) as a key first for better performance.  

## **Template Parameters**

***KeyType***
:   A type with which a [`basic_node`](index.md) object can be constructible.

***BasicNodeType***
:   A basic_node template instance type.

## **Parameters**

***`key`*** [in]
:   A key to check existence in the queried mapping.

## **Return Value**

`true` if the key exists in the queried mapping, `false` otherwise.  

??? Example

    ```cpp
    --8<-- "apis/basic_node/contains.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/contains.output"
    ```

## **See Also**

* [basic_node](index.md)
* [mapping_type](mapping_type.md)
* [type_error](../exception/type_error.md)
