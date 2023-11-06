<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_yaml_version

```cpp
yaml_version_t get_yaml_version() const noexcept;
```

Returns the version of the YAML format applied for the `basic_node` object.  

### **Return Value**

The version of the YAML format applied to the basic_node object.

| YAML version | Return Value            |
| ------------ | ----------------------- |
| 1.1          | yaml_version_t::VER_1_1 |
| 1.2          | yaml_version_t::VER_1_2 |

???+ Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {


        return 0;
    }
    ```

## **See Also**

* [basic_node](index.md)
* [yaml_verion_t](yaml_version_t.md)
* [set_yaml_version](set_yaml_version.md)
