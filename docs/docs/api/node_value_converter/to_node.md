<small>Defined in header [`<fkYAML/node_value_converter.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node_value_converter.hpp)</small>

# <small>fkyaml::node_value_converter::</small>from_node

```cpp
template <typename BasicNodeType, typename TargetType = ValueType>
static auto to_node(BasicNodeType& n, TargetType&& val) noexcept(
    noexcept(::fkyaml::to_node(n, std::forward<TargetType>(val))))
    -> decltype(::fkyaml::to_node(n, std::forward<TargetType>(val)))
```

Converts a value to a [`basic_node`](../basic_node/index.md) object.  
This function is usually called by the [`constructors`](../basic_node/constructor.md) of the [`basic_node`](../basic_node/index.md) class.  

!!! Tips "Customization for non-standard types"

    This function can be used for user-defined types by implementing (partial) specialization for `to_node()` function which is called internally by this function.  
    Note that the specialization **must be implemented in the same namespace as the user-defined types** so that the specialization can successfully be found by [ADL (Argument-Dependent Lookup)](https://en.cppreference.com/w/cpp/language/adl).  
    See the example below to get the feel of how it can be achieved.  

## **Template Parameters**

***BasicNodeType***
:   A basic_node template instance type.

***TargetType***
:   A target native data type.

## **Parameters**

***`n`*** [out]
:   A basic_node object to which the converted value is assigned.

***`val`*** [in]
:   A native data object used for conversion.

## **Return Value**

A basic_node object converted from a value of a compatible type.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/node_value_converter/to_node.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/node_value_converter/to_node.output"
    ```

## **See Also**

* [node](../basic_node/node.md)
* [basic_node::(constructor)](../basic_node/constructor.md)
* [basic_node::serialize](../basic_node/serialize.md)
* [operator<<(basic_node)](../basic_node/insertion_operator.md)
