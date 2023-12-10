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

Access to a YAML node element with either a key or an index.  
If the node is neither a mapping nor a sequence, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

!!! Danger

    This API does not check the size of a sequence node before accessing the element.  
    To avoid undefined behaviors, please make sure the argument `index` is smaller than the actual sequence size with a return value of the [`basic_node::size()`](size.md) function.  

!!! Warning

    This API does not check the existence of the given key in the YAML mapping node.  
    If the given key does not exist, a default [basic_node](index.md) object will be created.  
    Please make sure that the node has the given key in advance by calling the [`basic_node::contains()`](contains.md) function.  

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
If the node is a scalar, or if it is a sequence but the key is not an integer, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

### **Parameters**

***`index`*** [in]
:   An index/key for an element in the YAML sequence/mapping node.  

### **Return Value**

Reference, or constant reference, to the YAML node object associated with the given index/key.  

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create a YAML sequence node.
        fkyaml::node n1 = {123, 234, 345, 456};

        // print YAML nodes at the following indexes.
        std::cout << n1[0] << std::endl;
        std::cout << n1[1] << std::endl;
        std::cout << n1[2] << std::endl;
        std::cout << n1[3] << std::endl;

        // create a YAML mapping node.
        fkyaml::node n2 = {{"foo", true}, {"bar", 123}};

        // print YAML nodes associated with the following keys.
        std::cout << std::boolalpha << n2["foo"] << std::endl;
        std::cout << n2["bar"] << std::endl;
        return 0;
    }
    ```
    output:
    ```bash
    123
    234
    345
    456
    true
    123
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
If the node is a scalar, or if it is a sequence but the key is not an integer, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

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
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create a YAML sequence node.
        fkyaml::node n1 = {123, 234, 345, 456};

        // print YAML nodes at the following indexes.
        std::cout << n1[0] << std::endl;
        std::cout << n1[1] << std::endl;
        std::cout << n1[2] << std::endl;
        std::cout << n1[3] << std::endl;

        // create a YAML node.
        fkyaml::node n2 = {{"foo", true}, {"bar", 123}};

        // print YAML nodes associated with the following keys.
        std::cout << std::boolalpha << n2[fkyaml::node("foo")] << std::endl;
        std::cout << n2[fkyaml::node("bar")] << std::endl;

        return 0;
    }
    ```
    output:
    ```bash
    123
    234
    345
    456
    true
    123
    ```

## **See Also**

* [basic_node](index.md)
* [size](size.md)
* [contains](contains.md)
* [operator<<](insertion_operator.md)
* [type_error](../exception/type_error.md)
