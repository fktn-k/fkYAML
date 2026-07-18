<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_value_or

```cpp
template <typename T, typename U>
T get_value_or(U&& default_value) const noexcept;
```

This function tries to convert a [`fkyaml::basic_node`](./index.md) to `T`.  
Visit the documentation for the [get_value](./get_value.md) function for supported types since this function internally calls it.  
If the conversion fails, this function returns `default_value` instead of throwing an exception as the [`get_value`](./get_value.md) function does.  

Just as the [`get_value`](./get_value.md) function, this function also makes a copy of the value.  
If the copying costs too much, or if you need an address of the original value, then you should call one of the following functions instead.  

* [`as_seq`](as_seq.md)
* [`as_map`](as_map.md)
* [`as_bool`](as_bool.md)
* [`as_int`](as_int.md)
* [`as_float`](as_float.md)
* [`as_str`](as_str.md)

## **Template Parameters**

***T***
:   A compatible value type which might be cv-qualified or a reference type.  

***U***
:   The default value type from which `T` (the 1st overload) or `BasicNodeType` (the 2nd overload) must be constructible.  
    This is likely to be `const T&` or `T&&`.  

***BasicNodeType***
:   A basic_node template instance type.  

## **Return Value**

A value converted from the [basic_node](./index.md) object if the conversion succeeded, `default_value` otherwise.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/get_value_or.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/get_value_or.output"
    ```

## **See Also**

* [basic_node](index.md)
* [get_value](get_value.md)
* [as_seq](as_seq.md)
* [as_map](as_map.md)
* [as_bool](as_bool.md)
* [as_int](as_int.md)
* [as_float](as_float.md)
* [as_str](as_str.md)
* [node_value_converter::from_node](../node_value_converter/from_node.md)
