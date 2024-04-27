<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator>

```cpp
bool operator>(const basic_node& rhs) const noexcept;
```

Greater-than operator.  
Check if `this` object is greater than `rhs`.  
The operator returns the result of `!(*this <= rhs)`. (see [`operator<=`](operator_le.md))  

## **Parameters**

***`rhs`*** [in]
:   A `basic_node` object to be compared with `this` object.

## **Return Value**

`true` if `this` is greater than `rhs`, `false` otherwise.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_operator_gt.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_operator_gt.output"
    ```

## **See Also**

* [basic_node](index.md)
* [operator<=](operator_le.md)
