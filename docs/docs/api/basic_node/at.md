<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>at

```cpp
// (1)
template <typename KeyType>
basic_node& at(KeyType&& key);

template <typename KeyType>
const basic_node& at(KeyType&& key) const;

// (2)
template <typename BasicNodeType>
basic_node& at(BasicNodeType&& key);

template <typename BasicNodeType>
const basic_node& at(BasicNodeType&& key) const;
```

Access to an element in a container node with either an index or key value.  
This function must be called on a container node, or a [`fkyaml::type_error`](../exception/type_error.md) would be thrown.  

The input parameter `key` must be either a [`basic_node`](index.md) object or an object of a compatible type, i.e., a type with which a [`basic_node`](index.md) object can be constructible.  
Note that the overload (1) internally constructs a temporal [`basic_node`](index.md) object.  
So, if you use the same key multiple times, for example, in a for loop, consider creating a [`basic_node`](index.md) as a key first for better performance.

Furthermore, unlike the [`operator[]`](operator[].md), this function executes one of the following checks depending on the target node value type.  

* For sequence nodes  
    * Whether `key` is of an integral type (e.g., `int`, `size_t`) or an integer node.  
      If not, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.
    * Whether the value of `key` is more than or equal to the size of the queried sequence.  
      If not, a [`fkyaml::out_of_range`](../exception/out_of_range.md) will be thrown.
* For mapping nodes
    * Whether a given key exists in the target container.  
      If not, [`fkyaml::out_of_range`](../exception/out_of_range.md) will be thrown.

This function therefore costs a bit more than [`operator[]`](operator[].md) due to the above extra checks.  

## **Template Parameters**

***KeyType***
:   A compatible key type.

***BasicNodeType***
:   A basic_node template instance type.

## **Parameters**

***`key`*** [in]
:   A key to a target element in the sequence/mapping node.  

## **Return Value**

(Constant) reference to the node value which is associated with the given key.

## **Examples**

??? Example "Access an element with compatible keys"

    ```cpp
    --8<-- "apis/basic_node/at_compatible_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/at_compatible_type.output"
    ```

??? Example "Access an element with `basic_node` keys"

    ```cpp
    --8<-- "apis/basic_node/at_basic_node.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/at_basic_node.output"
    ```

## **See Also**

* [basic_node](index.md)
* [operator[]](operator[].md)
* [operator<<](insertion_operator.md)
* [out_of_range](../exception/out_of_range.md)
* [type_error](../exception/type_error.md)
