<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>contains

```cpp
template <
    typename KeyType, detail::enable_if_t<
                            detail::is_usable_as_key_type<
                                typename mapping_type::key_compare, typename mapping_type::key_type, KeyType>::value,
                            int> = 0>
bool contains(KeyType&& key) const;
```

Checks if the YAML node has the given key.  
If the node value is not a mapping, this API will throw an [`fkyaml::exception`](../exception/index.md).  

## **Template Parameters**

***KeyType***
:   A type compatible with the key type of mapping node values.

## **Parameters**

***`key`*** [in]
:   A key to the target value in the YAML mapping node value.

## **Return Value**

`true` if the YAML node is a mapping and has the given key, `false` otherwise.  

???+ Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create a YAML mapping node.
        fkyaml::node n = {{"foo", true}, {"bar", 123}};

        // check if the node has the following keys.
        std::cout << std::boolalpha;
        std::cout << n.contains("foo") << std::endl;
        std::cout << n.contains("baz") << std::endl;

        // create a YAML node. (not mapping)
        fkyaml::node n2 = "qux";

        // check if the node has the following key.
        std::cout << std::boolalpha << n2.contains("qux") << std::endl;

        return 0;
    }
    ```

    output:
    ```bash
    true
    false
    false
    ```

## **See Also**

* [basic_node](index.md)
* [mapping_type](mapping_type.md)
