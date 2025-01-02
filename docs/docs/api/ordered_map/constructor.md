<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/ordered_map.hpp)</small>

# <small>fkyaml::ordered_map::</small>(constructor)

```cpp
ordered_map(); // (1)

ordered_map(std::initializer_list<value_type> init); // (2)
```

Constructs a new ordered_map.  
You can specify the initial value on constructing an ordered_map with an overloaded constructor.  

## **Parameters**

***`init`*** [in]
:   An initializer list of key-value pairs.

## **Examples**

??? Example "Overload(1): create a default value."

    ```cpp
    --8<-- "apis/ordered_map/constructor_noarg.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/ordered_map/constructor_noarg.output"
    ```

??? Example "Overload(2): create an ordered_map object with an initializer list"

    ```cpp
    --8<-- "apis/ordered_map/constructor_initializer_list.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/ordered_map/constructor_initializer_list.output"
    ```

## **See Also**

* [ordered_map](index.md)
* [basic_node](../basic_node/index.md)
* [basic_node::(constructor)](../basic_node/constructor.md)
* [basic_node::serialize](../basic_node/serialize.md)
* [operator<<(basic_node)](../basic_node/insertion_operator.md)
