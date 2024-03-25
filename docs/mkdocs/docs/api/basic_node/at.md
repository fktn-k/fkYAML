<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>at

```cpp
template <
    typename KeyType, detail::enable_if_t<
                            detail::conjunction<
                                detail::negation<detail::is_basic_node<KeyType>>,
                                detail::is_node_compatible_type<basic_node, KeyType>>::value,
                            int> = 0>
basic_node& at(KeyType&& key); // (1)

template <
    typename KeyType, detail::enable_if_t<
                            detail::conjunction<
                                detail::negation<detail::is_basic_node<KeyType>>,
                                detail::is_node_compatible_type<basic_node, KeyType>>::value,
                            int> = 0>
const basic_node& at(KeyType&& key) const; // (2)

template <
    typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
basic_node& at(KeyType&& key); // (3)

template <
    typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
const basic_node& at(KeyType&& key) const; // (4)
```

Access to a YAML node element with either an index (for sequences) or a key (for mappings).  
Before accessing the element, this function checks the bounds in the case of a sequence or the existence of a key in the case of a mapping.  
This function therefore costs a bit more than [`basic_node::operator[]()`](operator[].md) function due to the extra checks.  
Furthermore, this function may throw the following exceptions:

* [`fkyaml::type_error`](../exception/type_error.md)
    * if the queried node is neither a sequence nor a mapping, or
    * if the queried node is a sequence but the given `key` is not an integer.
* [`fkyaml::out_of_range`](../exception/out_of_range.md)
    * if the given key does not exist in the queried mapping, or
    * if the given index exceeds the size of the queried sequence.

## Overload (1), (2)  

```cpp
template <
    typename KeyType, detail::enable_if_t<
                            detail::conjunction<
                                detail::negation<detail::is_basic_node<KeyType>>,
                                detail::is_node_compatible_type<basic_node, KeyType>>::value,
                            int> = 0>
basic_node& at(KeyType&& key); // (1)

template <
    typename KeyType, detail::enable_if_t<
                            detail::conjunction<
                                detail::negation<detail::is_basic_node<KeyType>>,
                                detail::is_node_compatible_type<basic_node, KeyType>>::value,
                            int> = 0>
const basic_node& at(KeyType&& key) const; // (2)
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
    --8<-- "examples/ex_basic_node_at_compatible_type.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_at_compatible_type.output"
    ```

## Overload (3), (4)

```cpp
template <
    typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
basic_node& at(KeyType&& key); // (3)

template <
    typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
const basic_node& at(KeyType&& key) const; // (4)
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
    --8<-- "examples/ex_basic_node_at_basic_node.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_at_basic_node.output"
    ```

## **See Also**

* [basic_node](index.md)
* [size](size.md)
* [contains](contains.md)
* [operator[]](operator[].md)
* [operator<<](insertion_operator.md)
* [out_of_range](../exception/out_of_range.md)
* [type_error](../exception/type_error.md)
