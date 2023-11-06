<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_value_ref

```cpp
template <typename ReferenceType, detail::enable_if_t<std::is_reference<ReferenceType>::value, int> = 0>
ReferenceType get_value_ref() const noexcept(
    noexcept(ConverterType<ValueType>::from_node(std::declval<const basic_node&>(), std::declval<ValueType&>())));

template <
    typename ReferenceType,
    detail::enable_if_t<
        detail::conjunction<
            std::is_reference<ReferenceType>, std::is_const<detail::remove_reference_t<ReferenceType>>>::value,
        int> = 0>
ReferenceType get_value_ref() const;
```

Explicit reference access to the internally stored YAML node value.  
This API makes no copies.  

## **Template Parameters**

***ReferenceType***
:   reference type to the target YAML node value.  
    This must be a reference to [`sequence_type`](sequence_type.md), [`mapping_type`](mapping_type.md), [`boolean_type`](boolean_type.md), [`integer_type`](integer_type.md), [`float_number_type`](float_number_type.md) or [`string_type`](string_type.md).  
    The above restriction is enforced by a static assertion.

## **Return Value**

Reference to the internally stored YAML node value if the requested reference type fits to the YAML node value.  
A [`fkyaml::exception`](../exception/index.md) would be thrown otherwise.  

!!! Note

    If this API throws an exception, the internally stored YAML node value stays intact.

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create a YAML node.
        fkyaml::node n = 123;

        // get references to the value.
        auto ref = n.get_value_ref<fkyaml::node::integer_type&>();
        auto cref = n.get_value_ref<const fkyaml::node::integer_type&>();

        // print the referenced values
        std::cout << ref << std::endl;
        std::cout << cref << std::endl;

        // specifying incompatible reference type throws an exception
        try
        {
            auto iref = value.get_value_ref<fkyaml::node::mapping_type&>();
        }
        catch (const fkyaml::exception& e)
        {
            std::cout << e.what() << std::endl;
        }

        return 0;
    }
    ```

    output:
    ```bash
    123
    123
    The node value is not a mapping.
    ```

## **See Also**

* [basic_node](index.md)
