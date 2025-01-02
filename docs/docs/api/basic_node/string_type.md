<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>string_type

```cpp
using string_type = StringType;
```

The type used to store integer node values and keys for mapping nodes.  

The type is defined by the template parameter `StringType`.  
If not explicitly specified, the default type `std::string` is defined.  
With the decided type, string values are stored directly inside a [`basic_node`](index.md).  

Note that string objects are stored as a pointer to an allocated memory area on the heap in a [`basic_node`](index.md) so that the internal storage size will at most be 8 bytes.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/string_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/string_type.output"
    ```

## **See Also**

* [basic_node](index.md)
