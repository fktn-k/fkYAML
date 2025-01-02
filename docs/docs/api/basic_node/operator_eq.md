<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator==

```cpp
bool operator==(const basic_node& rhs) const noexcept;
```

Compares two `basic_node` objects for equality according to the following rules:

* Two `basic_node` objects are equal if they are of the same [`node_type`](../node_type.md) type and their stored values are the same according to their respective `operator==`.
* Two `basic_node` objects are always equal if both of them are of the [`node_type::NULL_OBJECT`](../node_type.md) type.

## **Parameters**

***`rhs`*** [in]
:   A `basic_node` object to compare with.

## **Return Value**

`true` if both basic_node objects are equal, `false` otherwise.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/operator_eq.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/operator_eq.output"
    ```

## **See Also**

* [basic_node](index.md)
* [operator!=](operator_ne.md)
