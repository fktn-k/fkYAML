<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator==

```cpp
bool operator==(const basic_node& rhs) const noexcept;
```

Equal-to operator.  
Compares two `basic_node` objects for equality according to the following rules:  

* Two `basic_node` objects are equal if they are of the same [`node_t`](node_t.md) type and their stored values are the same according to their respective `operator==`.
* Two `basic_node` objects are always equal if both of them are of the [`node_t::NULL_OBJECT`](node_t.md) type.

## **Parameters**

***`rhs`*** [in]
:   A `basic_node` object to be compared with `this` object.

## **Return Value**

`true` if both types and values are equal, `false` otherwise.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_operator_eq.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_operator_eq.output"
    ```

## **See Also**

* [basic_node](index.md)
* [operator!=](operator_ne.md)
