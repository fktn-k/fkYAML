<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator[]

```cpp
// (1)
template <typename KeyType>
basic_node& operator[](KeyType&& key);

template <typename KeyType>
const basic_node& operator[](KeyType&& key) const;

// (2)
template <typename BasicNodeType>
basic_node& operator[](BasicNodeType&& key);

template <typename BasicNodeType>
const basic_node& operator[](BasicNodeType&& key) const;
```

Access to an element in a container node with either an index or key value.  
This function must be called on a container node, or a [`fkyaml::type_error`](../exception/type_error.md) would be thrown.  

The input parameter `key` must be either a [`basic_node`](index.md) object or an object of a compatible type, i.e., a type with which a [`basic_node`](index.md) object can be constructible.  

Note that the overload (1) internally constructs a temporal [`basic_node`](index.md) object.  
So, if you use the same key multiple times, for example, in a for loop, consider creating a [`basic_node`](index.md) as a key first for better performance.

!!! Danger "Accessing an element with an invalid index (Undefined Behavior)"

    This function does not check the size of the queried sequence before accessing the element.  
    To avoid undefined behaviors, please make sure the argument `key` is smaller than the queried sequence size with the [`size()`](size.md) function in advance.  
    Or use [`at()`](at.md) function instead, which executes the bounds check before accessing an element with a given index.

!!! Warning "Accessing an element with a non-existent key"

    This function does not check the existence of the given `key` in the queried mapping.  
    If the key does not exist, a [basic_node](index.md) object will be default-constructed.  
    If such a behavior is unwanted, choose one of the followings.
    * use [`at()`](at.md) function instead, which executes additional checks before accessing an element.
    * call the [`contains()`](contains.md) function in advance to make sure that the key exists in the queried mapping.

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

??? Example "Overload(1): access an element with compatible keys"

    ```cpp
    --8<-- "apis/basic_node/subscript_operator_compatible_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/subscript_operator_compatible_type.output"
    ```

??? Example "Overload(2): access an element with `basic_node` keys"

    ```cpp
    --8<-- "apis/basic_node/subscript_operator_basic_node.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/subscript_operator_basic_node.output"
    ```

## **See Also**

* [basic_node](index.md)
* [size](size.md)
* [contains](contains.md)
* [at](at.md)
* [operator<<](insertion_operator.md)
* [type_error](../exception/type_error.md)
