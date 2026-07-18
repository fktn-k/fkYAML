<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_value_ref

```cpp
template <typename ReferenceType>
ReferenceType get_value_ref();

template <typename ReferenceType>
ReferenceType get_value_ref() const;
```

!!! warning "Deprecation"

    This function has been deprecated in version 0.4.3 and replaced with the following functions since it offers too verbose interface and that may confuse library users, given that only non-/const reference types to [`sequence_type`](sequence_type.md), [`mapping_type`](mapping_type.md), [`boolean_type`](boolean_type.md), [`integer_type`](integer_type.md), [`float_number_type`](float_number_type.md) and [`string_type`](string_type.md) are accepted and that violating the limitation is not clear from error messages during compilation.  
    
    * [as_seq](as_seq.md)
    * [as_map](as_map.md)
    * [as_bool](as_bool.md)
    * [as_int](as_int.md)
    * [as_float](as_float.md)
    * [as_str](as_str.md)

    It will be removed in a future version. Please replace usages according to the following table.  

    | from                                                                             | to         |
    | -------------------------------------------------------------------------------- | ---------- |
    | get_value_ref<sequence_type&>(),<br>get_value_ref<const sequence_type&>()         | as_seq()   |
    | get_value_ref<mapping_type&>(),<br>get_value_ref<const mapping_type&>()           | as_map()   |
    | get_value_ref<boolean_type&>(),<br>get_value_ref<const boolean_type&>()           | as_bool()  |
    | get_value_ref<integer_type&>(),<br>get_value_ref<const integer_type&>()           | as_int()   |
    | get_value_ref<float_number_type&>(),<br>get_value_ref<const float_number_type&>() | as_float() |
    | get_value_ref<string_type&>(),<br>get_value_ref<const string_type&>()             | as_str()   |

Explicit reference access to the internally stored YAML node value.  
If the requested reference type does not match the current node value, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

!!! Note

    If this function throws an exception, the internally stored YAML node value stays intact.

## **Template Parameters**

***ReferenceType***
:   reference type to the target YAML node value.  
    It must be a (const) reference type to

    * [`sequence_type`](sequence_type.md)
    * [`mapping_type`](mapping_type.md)
    * [`boolean_type`](boolean_type.md)
    * [`integer_type`](integer_type.md)
    * [`float_number_type`](float_number_type.md)
    * [`string_type`](string_type.md)

## **Return Value**

Reference to the internally stored YAML node value.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/get_value_ref.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/get_value_ref.output"
    ```

## **See Also**

* [basic_node](index.md)
* [as_seq](as_seq.md)
* [as_map](as_map.md)
* [as_bool](as_bool.md)
* [as_int](as_int.md)
* [as_float](as_float.md)
* [as_str](as_str.md)
