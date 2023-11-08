<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator[]

```cpp
basic_node& operator[](std::size_t index); // (1)

const basic_node& operator[](std::size_t index) const; // (2)

template <
    typename KeyType, detail::enable_if_t<
                            detail::is_usable_as_key_type<
                                typename mapping_type::key_compare, typename mapping_type::key_type, KeyType>::value,
                            int> = 0>
basic_node& operator[](KeyType&& key); // (3)

template <
    typename KeyType, detail::enable_if_t<
                            detail::is_usable_as_key_type<
                                typename mapping_type::key_compare, typename mapping_type::key_type, KeyType>::value,
                            int> = 0>
const basic_node& operator[](KeyType&& key) const; // (4)
```

Access to a YAML node element with either a key or an index.  
If the node is neither a mapping nor a sequence, a [`fkyaml::exception`](../exception/index.md) will be thrown.  

## Overload (1), (2)  

```cpp
basic_node& operator[](std::size_t index); // (1)
const basic_node& operator[](std::size_t index) const; // (2)
```

Accesses to an element in the YAML sequence node with the given index.  
If the node is not a sequence, a [`fkyaml::exception`](../exception/index.md) will be thrown.  

!!! Danger

    This API does not check the size of a sequence node before accessing the element.  
    To avoid undefined behaviors, please make sure the argument `index` is smaller than the actual sequence size with a return value of [`size()`](size.md).  

### **Parameters**

***`index`*** [in]
:   An index for an element in the YAML sequence node.  

### **Return Value**

Reference, or constant reference, to the YAML node at the given index.  

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create a YAML sequence node.
        fkyaml::node n = {123, 234, 345, 456};

        // print YAML nodes at the following indexes.
        std::cout << n[0] << std::endl;
        std::cout << n[1] << std::endl;
        std::cout << n[2] << std::endl;
        std::cout << n[3] << std::endl;
        return 0;
    }
    ```

## Overload (3), (4)

```cpp
template <
    typename KeyType, detail::enable_if_t<
                            detail::is_usable_as_key_type<
                                typename mapping_type::key_compare, typename mapping_type::key_type, KeyType>::value,
                            int> = 0>
basic_node& operator[](KeyType&& key); // (3)

template <
    typename KeyType, detail::enable_if_t<
                            detail::is_usable_as_key_type<
                                typename mapping_type::key_compare, typename mapping_type::key_type, KeyType>::value,
                            int> = 0>
const basic_node& operator[](KeyType&& key) const; // (4)
```

Accesses to an element in the YAML mapping node with the given key.  
The given key must be of a compatible type with [`fkyaml::string_type`](string_type.md), i.e., a [`fkyaml::string_type`](string_type.md) object can be constructible with the given key.  
If the node is not a mapping, a [`fkyaml::exception`](../exception/index.md) will be thrown.  

!!! Warning

    This API does not check the existence of the given key in the YAML mapping node.  
    If the given key does not exist, a default [basic_node](index.md) object will be created.  
    Please make sure that the node has the given key beforehand by calling the [`contains`](contains.md) API.  

### **Template Parameters**

***KeyType***
:   A type compatible with the key type of mapping node values.

### **Parameters**

***key*** [in]
:   A key to the target value in the YAML mapping node.

### **Return Value**

Reference, or constant reference, to the YAML node associated with the given key.  

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create a YAML node.
        fkyaml::node n = {{"foo", true}, {"bar", 123}};

        // print YAML nodes associated with the following keys.
        std::cout << std::boolalpha << n["foo"] << std::endl;
        std::cout << n["bar"] << std::endl;

        return 0;
    }
    ```

## **See Also**

* [basic_node](index.md)
* [size](size.md)
* [contains](contains.md)
* [operator<<](insertion_operator.md)
