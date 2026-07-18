<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>set_yaml_version

```cpp
void set_yaml_version(const yaml_version_t version) noexcept;
```

!!! warning "Deprecation"

    The function [`#!cpp void set_yaml_version_type(const yaml_version_type)`](set_yaml_version_type.md) replaces the function `#!cpp void set_yaml_version(const basic_node::yaml_version_t)` which has been deprecated in version 0.3.12. It will be removed in a future version. Please replace calls like  
    
    ```cpp
    n.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_2);
    ```
    
    with  
    
    ```cpp
    n.set_yaml_version_type(fkyaml::yaml_version_type::VERSION_1_2);
    ```

Sets a target YAML specification version to the `basic_node` object.  

## **Parameters**

***version*** [in]
:   A target YAML version.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/set_yaml_version.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/set_yaml_version.output"
    ```

## **See Also**

* [basic_node](index.md)
* [yaml_version_t](yaml_version_t.md)
* [set_yaml_version](set_yaml_version.md)
