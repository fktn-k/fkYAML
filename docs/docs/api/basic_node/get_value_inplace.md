<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_value_inplace

```cpp
template <typename T>
void get_value_inplace(T& value_ref) const noexcept(
    noexcept(ConverterType<T>::from_node(std::declval<const basic_node&>(), std::declval<T&>()))); // (1)

template <typename BasicNodeType>
void get_value_inplace(BasicNodeType& value_ref) const; // (2)
```

This function converts a [`fkyaml::basic_node`](./index.md) to either of the followings and fills the conversion result into the given `value_ref` parameter.  

1. a compatible value ([copy-constructible](https://en.cppreference.com/w/cpp/named_req/CopyConstructible))  
   The function is equivalent to executing  
   ```cpp
   ConverterType<T>::from_node(*this, value_ref);
   ```
   Unlike the [`get_value`](./get_value.md) function, this function does not require the template parameter type `T` to be [default-constructible](https://en.cppreference.com/w/cpp/named_req/DefaultConstructible) since no instantiation of `T` is necessary inside the function to return the conversion result.  
2. a [fkyaml::basic_node](./index.md) object  
   The function is equivalent to executing  
   ```cpp
   value_ref = *this; // Copy-assigns a current basic_node object.
   ```

This function shares internal implementation with the [`get_value()`](./get_value.md) function.  
Thus, all the STL container & scalar types which are supported by that function, are also supported by this function as well.  
See the notes there for details.  

??? Note "Differences from `get_value()`"

    One crucial difference from the [`get_value`](./get_value.md) function is, this function does not require the template parameter type `T` to be [default-constructible](https://en.cppreference.com/w/cpp/named_req/DefaultConstructible) since no instantiation of `T` is necessary inside the function to return the conversion result anymore.  
    So, you might prefer this function, for example, if you already created a `T` object as a member variable and want to assign a node value to it.  
    
    Another is C-style array support.  
    While [`get_value`](./get_value.md) cannot accept any kind of C-style array types since returning such array objects is impossible due to its implementation, this function accepts 1D, 2D and 3D arrays. Note that, if `T` is one of them, the target basic_node object must be a sequence. A [`type_error`](../exception/type_error.md) is thrown otherwise.

## **Template Parameters**

***T***
:   A compatible value type.  

***BasicNodeType***
:   A basic_node template instance type.  

### **Parameters**

***`value_ref`*** [out]
:   A storage into which the conversion result is filled.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/get_value_inplace.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/get_value_inplace.output"
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
