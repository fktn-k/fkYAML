<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/ordered_map.hpp)</small>

# <small>fkyaml::ordered_map::</small>operator[]

```cpp
template <
    typename KeyType,
    detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
mapped_type& operator[](KeyType&& key) noexcept;
```

Accesses an element with the given key.  

!!! Tip

    This API behaves like the [`std::map`](https://en.cppreference.com/w/cpp/container/map) class does.  
    In other words, it does not check the existence of the given key, and could return a default value if the key does not exist.  
    To avoid such a behavior, you can use [`at()`](at.md) function which throws a [`fkyaml::exception`](../exception/index.md) if the given key does not match any keys in the ordered_map object.

## **Template Parameters**

***KeyType***
:   A type compatible with the key type.

## **Parameters**

***`key`*** [in]
:   A key to the target value.

## **Return Value**

Reference to a `mapped_type` object associated with the given key.  
Possibly a default value of the `mapped_type` if the ordered_map does not contain the given key.  

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>
    #include <fkYAML/ordered_map.hpp>

    int main()
    {
        fkyaml::ordered_map<std::string, fkyaml::node> om = {
            { "foo", 123 },
            { "bar", "baz" }
        };

        std::cout << fkyaml::node::serialize(om["foo"]) << std::endl;
        std::cout << fkyaml::node::serialize(om["bar"]) << std::endl;

        // accesses with a unknown key will create a new key-value pair with the default value.
        std::cout << fkyaml::node::serialize(om["qux"]) << std::endl;

        return 0;
    }
    ```

    output:
    ```bash
    123
    baz
    null
    ```

## **See Also**

* [ordered_map](index.md)
* [at](at.md)
* [node](../basic_node/node.md)
* [basic_node::serialize](../basic_node/serialize.md)
