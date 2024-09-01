<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>set_yaml_version_type

```cpp
void set_yaml_version_type(const yaml_version_type version) noexcept;
```

Sets a target YAML version to the `basic_node` object.  

### **Parameters**

***version*** [in]
:   A target YAML version.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_set_yaml_version_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_set_yaml_version_type.output"
    ```

## **See Also**

* [basic_node](index.md)
* [get_yaml_version_type](get_yaml_version_type.md)
* [yaml_verion_type](../yaml_version_type.md)
