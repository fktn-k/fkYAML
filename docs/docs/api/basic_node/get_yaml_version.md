<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_yaml_version

```cpp
yaml_version_t get_yaml_version() const noexcept;
```

!!! warning "Deprecation"

    The function [`#!cpp yaml_version_type get_yaml_version_type()`](set_yaml_version_type.md) replaces the function `#!cpp basic_node::yaml_version_t get_yaml_version()` which has been deprecated in version 0.3.12. It will be removed in a future version. Please replace calls like  
    
    ```cpp
    fkyaml::node::yaml_version_t v = n.get_yaml_version();
    ```
    with
    
    ```cpp
    fkyaml::yaml_version_type v = n.get_yaml_version_type();
    ```

Returns the YAML specification version applied for a `basic_node`.  

## **Return Value**

The YAML specification version applied to a basic_node.

| YAML version | Return Value            |
| ------------ | ----------------------- |
| 1.1          | yaml_version_t::VER_1_1 |
| 1.2          | yaml_version_t::VER_1_2 |

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/get_yaml_version.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/get_yaml_version.output"
    ```

## **See Also**

* [basic_node](index.md)
* [yaml_version_t](yaml_version_t.md)
* [set_yaml_version](set_yaml_version.md)
