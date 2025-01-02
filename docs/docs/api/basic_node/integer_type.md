<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>integer_type

```cpp
using integer_type = IntegerType;
```

The type used to store integer values.  

The actual type is defined by the template parameter `IntegerType`.  
If not explicitly specified, the default type `std::int64_t` is defined.  
With the decided type, integer values are stored directly inside a [`basic_node`](index.md).  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/integer_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/integer_type.output"
    ```

## **See Also**

* [basic_node](index.md)
