<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator>

```cpp
bool operator>(const basic_node& rhs) const noexcept;
```

Check if `this` object is greater than `rhs`.  

## **Parameters**

***`rhs`*** [in]
:   A `basic_node` object to compare with.

## **Return Value**

`true` if `this` is greater than `rhs`, `false` otherwise.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/operator_gt.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/operator_gt.output"
    ```

## **See Also**

* [basic_node](index.md)
* [operator<=](operator_le.md)
