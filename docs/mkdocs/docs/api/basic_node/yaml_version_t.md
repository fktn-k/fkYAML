<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>yaml_version_t

```cpp
enum class yaml_version_t
{
    VER_1_1,
    VER_1_2,
};
```

!!! warning "Deprecation"

    The enum class [`fkyaml::yaml_version_type`](../yaml_version_type.md) replaces the type alias `fkyaml::basic_node::yaml_version_t` which has been deprecated in version 0.3.12. It will be removed in version 0.4.0. Please replace usages like  
    
    ```cpp
    fkyaml::node::yaml_version_t v;
    ```
    
    with  
    
    ```cpp
    fkyaml::yaml_version_type v;
    ```

This enumeration collects the used versions of YAML specification. It is used as meta data of a basic_node and the functions [`get_yaml_version`](get_yaml_version.md) and [`set_yaml_version`](set_yaml_version.md) rely on it.  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_yaml_version_t.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_yaml_version_t.output"
    ```

### **See Also**

* [deserialize](deserialize.md)
* [get_yaml_version](get_yaml_version.md)
* [set_yaml_version](set_yaml_version.md)
