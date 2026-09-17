<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>mapping_type

```cpp
using mapping_type = MappingType<basic_node, basic_node>;
```

The type used to store mapping values.  

The actual type is defined by the template parameter `MappingType`.  
If not explicitly specified, the default type `std::map` is defined.  
With the decided container type, the type of mapping objects will then be decided in the form of `MappingType<basic_node, basic_node>` with which mapping objects are stored inside a [`basic_node`](index.md).  

Note that mapping values are stored as a pointer to an allocated memory area on the heap in a [`basic_node`](index.md) so that the internal storage size will at most be 8 bytes.  

!!! Note "Preserve the insertion order of key-value pairs"

    The YAML specification describes as a mapping node as "an unordered association of unique keys to values".  
    Which means that **the insertion order of key-value pairs are not guaranteed by the YAML specification**.  
    To follow the specification, the fkYAML library uses `std::map` as the mapping container type by default with the intention of better integration with other C++ programs.  
    If mapping values needs to be dependent on the insertion order, you can force preserving the insertion order by specifying [`fkyaml::ordered_map`](../ordered_map/index.md) as the MappingType template parameter as follows:

    ```cpp
    #include <fkYAML/node.hpp>
    using ordered_node = basic_node<std::vector, fkyaml::ordered_map>;
    ```

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/mapping_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/mapping_type.output"
    ```

## **See Also**

* [basic_node](index.md)
* [ordered_map](../ordered_map/index.md)
