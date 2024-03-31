<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator[]

```cpp
template <
    typename KeyType, detail::enable_if_t<
                            detail::conjunction<
                                detail::negation<detail::is_basic_node<KeyType>>,
                                detail::is_node_compatible_type<basic_node, KeyType>>::value,
                            int> = 0>
basic_node& operator[](KeyType&& key); // (1)

template <
    typename KeyType, detail::enable_if_t<
                            detail::conjunction<
                                detail::negation<detail::is_basic_node<KeyType>>,
                                detail::is_node_compatible_type<basic_node, KeyType>>::value,
                            int> = 0>
const basic_node& operator[](KeyType&& key) const; // (2)

template <
    typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
basic_node& operator[](KeyType&& key); // (3)

template <
    typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
const basic_node& operator[](KeyType&& key) const; // (4)
```

Access to a YAML node element with either an index (for sequences) or a key (for mappings).  
If the node is neither a sequence nor a mapping, or if the node is a sequence but the given `key` is not an integer, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

!!! Danger

    This API does not check the size of a sequence node before accessing the element.  
    To avoid undefined behaviors, please make sure the argument `key` is smaller than the actual sequence size with a return value of the [`basic_node::size()`](size.md) function.  
    If bounds check is necessary, prefer [`basic_node::at()`](at.md) function.

!!! Warning

    This API does not check the existence of the given `key` in a mapping node.  
    If the key does not exist, a default [basic_node](index.md) object will be created.  
    Please make sure that the node has the given key in advance by calling the [`basic_node::contains()`](contains.md) function.  
    If such a behavior is unwanted, prefer [`basic_node::at()`](at.md) function.

## Overload (1), (2)  

```cpp
template <
    typename KeyType, detail::enable_if_t<
                            detail::conjunction<
                                detail::negation<detail::is_basic_node<KeyType>>,
                                detail::is_node_compatible_type<basic_node, KeyType>>::value,
                            int> = 0>
basic_node& operator[](KeyType&& key); // (1)

template <
    typename KeyType, detail::enable_if_t<
                            detail::conjunction<
                                detail::negation<detail::is_basic_node<KeyType>>,
                                detail::is_node_compatible_type<basic_node, KeyType>>::value,
                            int> = 0>
const basic_node& operator[](KeyType&& key) const; // (2)
```

Accesses to an element in the YAML sequence/mapping node with the given key object of a compatible type with the [`basic_node`](index.md) class, i.e., a type with which a [`basic_node`](index.md) object is constructible.  
These overloads internally construct a [`basic_node`](index.md) object with `key`.  

### **Parameters**

***`index`*** [in]
:   An index/key for an element in the YAML sequence/mapping node.  

### **Return Value**

Reference, or constant reference, to the YAML node object associated with the given index/key.  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_subscript_operator_compatible_type.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_subscript_operator_compatible_type.output"
    ```

## Overload (3), (4)

```cpp
template <
    typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
basic_node& operator[](KeyType&& key); // (3)

template <
    typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
const basic_node& operator[](KeyType&& key) const; // (4)
```

Accesses to an element in the YAML sequence/mapping node with the given [`basic_node`](index.md) key object.  
Unlike the overloads (1) and (2) above, these overloads do not internally construct a [`basic_node`](index.md) object.  
So, these overloads works more effectively when some key objects are used multiple times, for instance, in a for-loop.  

### **Template Parameters**

***KeyType***
:   A key type which is a kind of the [`basic_node`](index.md) template class.

### **Parameters**

***`key`*** [in]
:   An index/key for an element in the YAML sequence/mapping node.

### **Return Value**

Reference, or constant reference, to the YAML node object associated with the given index/key.  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_subscript_operator_basic_node.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_subscript_operator_basic_node.output"
    ```

## **See Also**

* [basic_node](index.md)
* [size](size.md)
* [contains](contains.md)
* [at](at.md)
* [operator<<](insertion_operator.md)
* [type_error](../exception/type_error.md)
