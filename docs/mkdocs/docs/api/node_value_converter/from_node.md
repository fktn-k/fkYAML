<small>Defined in header [`<fkYAML/node_value_converter.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node_value_converter.hpp)</small>

# <small>fkyaml::basic_node::</small>from_node

```cpp
template <typename BasicNodeType, typename TargetType = ValueType>
static auto from_node(BasicNodeType&& n, TargetType& val) noexcept(
    noexcept(::fkyaml::from_node(std::forward<BasicNodeType>(n), val)))
    -> decltype(::fkyaml::from_node(std::forward<BasicNodeType>(n), val), void())
```

Converts a [`basic_node`](../basic_node/index.md) object to the target native data object.  
This function is usually called by the [`get_value()`](../basic_node/get_value.md) function of the [`basic_node`](../basic_node/index.md) class.  
Note that the `TargetType` must be default-constructible.  

!!! Tips

    This function can be used for user-defined types by implementing (partial) specialization for `from_node()` function which is called internally by this function.  
    Note that the specialization **must be implemented in the same namespace as the user-defined types** so that the specialization can successfully be found by ADL (Argument Dependent Lookup).  
    See the example below for more information.  

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

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    namespace ns
    {

    struct book
    {
        std::string title;
        std::string author;
        int year;
    };

    void from_node(const fkyaml::node& n, book& b)
    {
        b.title  = n["title"].get_value_ref<const fkyaml::node::string_type&>();
        b.author = n["author"].get_value_ref<const fkyaml::node::string_type&>();
        b.year   = n["year"].get_value<int>();
    }

    } // namespace ns

    int main()
    {
        fkyaml::node n = {
            { "title", "Noman's Journey" },
            { "author", "John Doe" },
            { "year", 2023 },
        };

        auto b = n.get_value<ns::book>();

        std::cout << "\"" << b.title << "\" was written by " << b.author
                  << " in " << b.year << "." << std::endl;

        return 0;
    }
    ```

    output:
    ```bash
    "Noman's Journey" was written by John Doe in 2023.
    ```

## **See Also**

* [node](../basic_node/node.md)
* [basic_node::get_value](../basic_node/get_value.md)
* [basic_node::get_value_ref](../basic_node/get_value_ref.md)
