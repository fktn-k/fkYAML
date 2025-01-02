<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator<

```cpp
bool operator<(const basic_node& rhs) const noexcept;
```

Check if `this` object is less than `rhs` according to the following rules:  

* If the values are of the same [`node_type`](../node_type.md) type, their stored values are compared according to their respective `operator<`.
* Two `basic_node` objects are always equal if both of them are of the [`node_type::NULL_OBJECT`](../node_type.md) type.
* If the values are of different [`node_type`](../node_type.md) types, the stored values are ignored and the order of the types are considered. The order of the types is as followed (top < bottom):
    * node_type::SEQUENCE
    * node_type::MAPPING
    * node_type::NULL_OBJECT
    * node_type::BOOLEAN
    * node_type::INTEGER
    * node_type::FLOAT
    * node_type::STRING
* If the values are of the [`node_type::BOOLEAN`](../node_type.md) type, a value whose stored value is `false` is less than a value whose stored value is `true`.

## **Parameters**

***`rhs`*** [in]
:   A `basic_node` object to compared with.

## **Return Value**

`true` if `this` is less than `rhs`, `false` otherwise.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/operator_lt.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/operator_lt.output"
    ```

## **See Also**

* [basic_node](index.md)
* [operator==](operator_eq.md)
