<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/ordered_map.hpp)</small>

# <small>fkyaml::ordered_map::</small>(constructor)

```cpp
ordered_map(); // (1)

ordered_map(std::initializer_list<value_type> init); // (2)
```

Constructs a new ordered_map.  
You can specify the initial value on constructing an ordered_map with an overloaded constructor.  

## Overload (1)

```cpp
ordered_map(); // (1)
```

Constructs an ordered_map object without an initial value.  
The content of a newly constructed ordered_map is an empty list of key-value pairs.  

???+ Example

    ```cpp
    --8<-- "examples/ex_ordered_map_constructor_noarg.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_ordered_map_constructor_noarg.output"
    ```

## Overload (2)

```cpp
ordered_map(std::initializer_list<value_type> init); // (2)
```

Constructs a new ordered_map with an initializer list.  
The resulting ordered_map object has the same list of key-value pairs as the given initializer list.  

## **Parameters**

***`init`*** [in]
:   An initializer list of key-value pairs.

???+ Example

    ```cpp
    --8<-- "examples/ex_ordered_map_constructor_initializer_list.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_ordered_map_constructor_initializer_list.output"
    ```

## **See Also**

* [ordered_map](index.md)
* [basic_node](../basic_node/index.md)
* [basic_node::(constructor)](../basic_node/constructor.md)
* [basic_node::serialize](../basic_node/serialize.md)
* [operator<<(basic_node)](../basic_node/insertion_operator.md)
