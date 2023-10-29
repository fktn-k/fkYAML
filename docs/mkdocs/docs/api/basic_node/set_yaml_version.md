<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>set_yaml_version

```cpp
void set_yaml_version(const yaml_version_t version) noexcept;
```

Sets the version of the YAML format to the `basic_node` object.  

### **Parameters**

***version*** [in]
:   A version of the YAML format.

### **Return Values**

| YAML version | Return Value            |
| ------------ | ----------------------- |
| 1.1          | yaml_version_t::VER_1_1 |
| 1.2          | yaml_version_t::VER_1_2 |

??? Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        fkyaml::node n;
        n.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_1);
        fkyaml::node n2;
        n2.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_2);

        std::cout << std::boolalpha;
        std::cout << n.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_1 << std::endl;
        std::cout << n2.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2 << std::endl;

        return 0;
    }
    ```

    output:
    ```bash
    true
    true
    ```

## **See Also**

* [basic_node](index.md)
* [yaml_verion_t](yaml_version_t.md)
* [set_yaml_version](set_yaml_version.md)