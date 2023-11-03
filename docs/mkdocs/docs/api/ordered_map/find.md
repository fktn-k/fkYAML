<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/ordered_map.hpp)</small>

# <small>fkyaml::ordered_map::</small>find

```cpp
template <
    typename KeyType,
    detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
iterator find(KeyType&& key) noexcept;

template <
    typename KeyType,
    detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
const_iterator find(KeyType&& key) const noexcept;
```

Find a value with the given key.  

## **Template Parameters**

***KeyType***
:   A type compatible with the key type.

## **Parameters**

***`key`*** [in]
:   A key to the target value.

## **Return Value**

An iterator to the target value if found, the result of end() otherwise.  

???+ Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>
    #include <fkYAML/ordered_map.hpp>

    int main()
    {
        fkyaml::ordered_map<std::string, fkyaml::node> om = {
            { "foo", 123 },
            { "bar", "baz" }
        };

        // search for a value with an existing key.
        auto itr = om.find("foo");
        if (itr != om.end())
        {
            std::cout << fkyaml::node::serialize(*itr) << std::endl;
        }

        // search for a value with a key which does not exist.
        auto itr2 = om.emplace("qux");
        if (itr2 == om.end())
        {
            std::cout << "key does not exist." << std::endl;
        }

        return 0;
    }
    ```

    output:
    ```bash
    123
    key does not exist.
    ```

## **See Also**

* [ordered_map](index.md)
* [node](../basic_node/node.md)
* [basic_node::serialize](../basic_node/serialize.md)
