<small>Defined in header [`<fkYAML/yaml_version_type.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/yaml_version_type.hpp)</small>

# <small>fkyaml::</small>yaml_version_type

```cpp
enum class yaml_version_type
{
    VERSION_1_1,
    VERSION_1_2,
};
```

This enumeration collects the used versions of YAML specification. It is used as meta data of a basic_node and the following [basic_node](basic_node/index.md)'s functions rely on it:

* [`get_yaml_version_type()`](basic_node/get_yaml_version_type.md)
* [`set_yaml_version_type()`](basic_node/set_yaml_version_type.md)

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/yaml_version_type/yaml_version_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/yaml_version_type/yaml_version_type.output"
    ```

## **See Also**

* [deserialize](basic_node/deserialize.md)
* [get_yaml_version_type](basic_node/get_yaml_version_type.md)
* [set_yaml_version_type](basic_node/set_yaml_version_type.md)
