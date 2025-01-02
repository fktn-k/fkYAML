<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>value_converter_type

```cpp
template <typename T, typename SFINAE>
using value_converter_type = ValueConverterType<T, SFINAE>;
```

A helper type alias to determine converter type for the target type `T`.  
The default type of the template parameter `ValueConverterType` is [`node_value_converter`](../node_value_converter/index.md).  
This type alias is used to determine the target type `T` is convertible from/to [`basic_node`](index.md) and quite a few functions rely on it such as the [`get_value()`](get_value.md) function.  

If you want to convert some type from/to `basic_node`, however, it is recommended to use [`basic_node::get_value`](get_value.md) instead, in order for your program to avoid too much dependency on the fkYAML library implementation.  

## **Template Parameters**

***T***
: The target native data type.

***SFINAE***
: Type for [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae). If you do not need compile-time type checks, pass just `void`.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/value_converter_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/value_converter_type.output"
    ```

## **See Also**

* [basic_node](index.md)
* [node_value_converter](../node_value_converter/index.md)
