<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>set_yaml_version

```cpp
void set_yaml_version(const yaml_version_t version) noexcept;
```

Sets the version of the YAML format to the `basic_node` object.  

### **Parameters**

***version*** [in]
:   A version of the YAML format.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_set_yaml_version.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_set_yaml_version.output"
    ```

## **See Also**

* [basic_node](index.md)
* [yaml_verion_t](yaml_version_t.md)
* [set_yaml_version](set_yaml_version.md)
