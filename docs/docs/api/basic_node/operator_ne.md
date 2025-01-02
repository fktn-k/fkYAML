<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator!=

```cpp
bool operator!=(const basic_node& rhs) const noexcept;
```

Compares two `basic_node` objects for inequality.  

## **Parameters**

***`rhs`*** [in]
:   A `basic_node` object to compare with.

## **Return Value**

`true` if either types or values are not equal, `false` otherwise.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/operator_ne.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/operator_ne.output"
    ```

## **See Also**

* [basic_node](index.md)
* [operator==](operator_eq.md)
