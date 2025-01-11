<small>Defined in header [`<fkYAML/node_value_converter.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node_value_converter.hpp)</small>

# <small>fkyaml::node_value_converter::</small>from_node

```cpp
template <typename BasicNodeType, typename TargetType = ValueType>
static auto from_node(BasicNodeType&& n, TargetType& val) noexcept(
    noexcept(::fkyaml::from_node(std::forward<BasicNodeType>(n), val)))
    -> decltype(::fkyaml::from_node(std::forward<BasicNodeType>(n), val), void())
```

Converts a [`basic_node`](../basic_node/index.md) object to the target native data object.  
This function is usually called by the [`get_value()`](../basic_node/get_value.md) function of the [`basic_node`](../basic_node/index.md) class.  
Note that the `TargetType` must be default-constructible.  

!!! Tips "Customization for non-standard types"

    This function can be used for user-defined types by implementing (partial) specialization for `from_node()` function which is called internally by this function.  
    Note that the specialization **must be implemented in the same namespace as the user-defined types (including the global namespace)** so that the specialization can successfully be found by [ADL (Argument Dependent Lookup)](https://en.cppreference.com/w/cpp/language/adl).  
    You can find a detailed explanation of how this customization point works at [this link](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html).  
    Also, see the example below to get the feel of how it can be achieved.  

## **Template Parameters**

***BasicNodeType***
:   A basic_node template instance type.

***TargetType***
:   A target native data type.

## **Parameters**

***`n`*** [in]
:   A basic_node object used for conversion.

***`val`*** [out]
:   A native data object to which the converted value is assigned.

## **Return Value**

A value of a compatible type converted from a basic_node.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/node_value_converter/from_node.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/node_value_converter/from_node.output"
    ```

## **See Also**

* [node](../basic_node/node.md)
* [basic_node::get_value](../basic_node/get_value.md)
* [basic_node::get_value_ref](../basic_node/get_value_ref.md)
