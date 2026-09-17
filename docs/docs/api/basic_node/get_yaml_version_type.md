<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_yaml_version_type

```cpp
yaml_version_type get_yaml_version_type() const noexcept;
```

Returns the YAML specification version applied to a `basic_node`.  

## **Return Value**

The YAML specification version applied to a basic_node.

| YAML version | Return Value                   |
| ------------ | ------------------------------ |
| 1.1          | yaml_version_type::VERSION_1_1 |
| 1.2          | yaml_version_type::VERSION_1_2 |

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/get_yaml_version_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/get_yaml_version_type.output"
    ```

## **See Also**

* [basic_node](index.md)
* [set_yaml_version_type](set_yaml_version_type.md)
* [yaml_version_type](../yaml_version_type.md)
