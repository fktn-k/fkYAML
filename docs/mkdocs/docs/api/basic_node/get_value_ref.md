<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_value_ref

```cpp
template <typename ReferenceType>
ReferenceType get_value_ref();

template <typename ReferenceType>
ReferenceType get_value_ref() const;
```

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

??? Example

    ```cpp
    --8<-- "examples/ex_basic_node_get_value_ref.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_get_value_ref.output"
    ```

## **See Also**

* [basic_node](index.md)
