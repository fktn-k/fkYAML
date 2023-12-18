<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator!=

```cpp
bool operator!=(const basic_node& rhs) const noexcept;
```

Not-Equal-to operator.  
Compares two `basic_node` objects for inequality.  
This operator returns the result of `!(*this == rhs)`. (see [`operator==`](operator_eq.md))

## **Parameters**

***`rhs`*** [in]
:   A `basic_node` object to be compared with `this` object.

## **Return Value**

`true` if either types or values are not equal, `false` otherwise.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_operator_ne.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_operator_ne.output"
    ```

## **See Also**

* [basic_node](index.md)
* [operator==](operator_eq.md)
