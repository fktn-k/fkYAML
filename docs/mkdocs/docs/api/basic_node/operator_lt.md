<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator<

```cpp
bool operator<(const basic_node& rhs) const noexcept;
```

Less-than operator.  
Check if `this` object is less than `rhs` according to the following rules:  

* If the values are of the same [`node_t`](node_t.md) type, their stored values are compared according to their respective `operator<`.
* Two `basic_node` objects are always equal if both of them are of the [`node_t::NULL_OBJECT`](node_t.md) type.
* If the values are of different [`node_t`](node_t.md) types, the stored values are ignored and the order of the types are considered. The order of the types is as followed (top < bottom):
    * node_t::SEQUENCE
    * node_t::MAPPING
    * node_t::NULL_OBJECT
    * node_t::BOOLEAN
    * node_t::INTEGER
    * node_t::FLOAT_NUMBER
    * node_t::STRING
* If the values are of the [`node_t::BOOLEAN`](node_t.md) type, a value whose stored value is `false` is less than a value whose stored value is `true`.

## **Parameters**

***`rhs`*** [in]
:   A `basic_node` object to be compared with `this` object.

## **Return Value**

`true` if `this` is less than `rhs`, `false` otherwise.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_operator_lt.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_operator_lt.output"
    ```

## **See Also**

* [basic_node](index.md)
* [operator==](operator_eq.md)
