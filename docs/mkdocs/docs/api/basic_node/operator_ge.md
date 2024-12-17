<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator>=

```cpp
bool operator>=(const basic_node& rhs) const noexcept;
```

Check if `this` object is greater than or equal to `rhs`.  

## **Parameters**

***`rhs`*** [in]
:   A `basic_node` object to compare with.

## **Return Value**

`true` if `this` is greater than or equal to `rhs`, `false` otherwise.

??? Example

    ```cpp
    --8<-- "examples/ex_basic_node_operator_ge.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_operator_ge.output"
    ```

## **See Also**

* [basic_node](index.md)
* [operator<](operator_lt.md)
