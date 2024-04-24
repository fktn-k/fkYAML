<small>Defined in header [`<fkYAML/node_value_converter.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node_value_converter.hpp)</small>

# <small>fkyaml::basic_node::</small>from_node

```cpp
template <typename BasicNodeType, typename TargetType = ValueType>
static auto to_node(BasicNodeType& n, TargetType&& val) noexcept(
    noexcept(::fkyaml::to_node(n, std::forward<TargetType>(val))))
    -> decltype(::fkyaml::to_node(n, std::forward<TargetType>(val)))
```

Converts a native data to a [`basic_node`](../basic_node/index.md) object.  
This function is usually called by the constructors of the [`basic_node`](../basic_node/index.md) class.  

!!! Tips

    This function can be used for user-defined types by implementing (partial) specialization for `to_node()` function which is called internally by this function.  
    Note that the specialization **must be implemented in the same namespace as the user-defined types** so that the specialization can successfully be found by ADL (Argument Dependent Lookup).  
    See the example below for more information.  

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

???+ Example

    ```cpp
    --8<-- "examples/ex_node_value_converter_to_node.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_node_value_converter_to_node.output"
    ```

## **See Also**

* [node](../basic_node/node.md)
* [basic_node::(constructor)](../basic_node/constructor.md)
* [basic_node::serialize](../basic_node/serialize.md)
* [operator<<(basic_node)](../basic_node/insertion_operator.md)
