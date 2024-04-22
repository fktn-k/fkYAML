<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>mapping_type

```cpp
using mapping_type = MappingType<StringType, basic_node>;
```

The type used to store mapping node values.  

To store mapping objects in [`basic_node`](index.md) class, the container type is defined by the template parameter `MappingType` which chooses the type to use for integer objects.  
If not explicitly specified, the default type `std::map` will be chosen.  
With the decided container type, the type of mapping objects will then be decided in the form of `MappingType<StringType, basic_node>` with which mapping objects are stored inside a [`basic_node`](index.md).  
Note that mapping objects are stored as pointers in a [`basic_node`](index.md) so that the internal storage size will at most be 8 bytes.  

!!! Note "Preserve the insertion order of key-value pairs"

    The YAML specification describes as a mapping node as "an unordered association of unique keys to values".  
    Which means that **the insertion order of key-value pairs are not guranteed by the YAML specification**.  
    To follow the specification, the fkYAML library uses `std::map` as the mapping container type by default with the intention of better integration with other C++ programs.  
    If your YAML documents really needs to be dependent on the insertion order (the YAML specification discourages that though), you can force preserving the insertion order by specifying [`fkyaml::ordered_map`](../ordered_map/index.md) as the MappingType template parameter as follows:

    ```cpp
    #include <fkYAML/node.hpp>
    using ordered_node = basic_node<std::vector, fkyaml::ordered_map>;
    ```

### **Template Paramters**

`MappingType`
:   The container to store key-value pairs. Defaults to `std::map`.

`StringType`
:   The type of keys and string scalar values. Defaults to `std::string`.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_mapping_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_mapping_type.output"
    ```

### **See Also**

* [basic_node](index.md)
* [ordered_map](../ordered_map/index.md)
