<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>yaml_version_t

```cpp
enum class yaml_version_t
{
    VER_1_1,
    VER_1_2,
};
```

This enumeration collects the used versions of YAML specification. It is used as meta data of a basic_node and the functions [`get_yaml_version`](get_yaml_version.md) and [`set_yaml_version`](set_yaml_version.md) rely on it.  

???+ Example

    ```cpp
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        char input[] =
            "%YAML 1.2\n"
            "---\n"
            "foo: bar\n";

        // deserialize a YAML formatted string.
        fkyaml::node n = fkyaml::node::deserialize(input);

        // call get_yaml_version().
        fkyaml::node::yaml_version_t yaml_ver = n.get_yaml_version();
        std::cout << std::boolalpha;
        std::cout << (n.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2) << std::endl;

        // overwrite the YAML version to 1.1.
        n.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_1);

        // call get_yaml_version() again.
        std::cout << (n.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_1) << std::endl;

        return 0;
    }
    ```

    output:
    ```bash
    true
    true
    ```

### **See Also**

* [deserialize](deserialize.md)
* [get_yaml_version](get_yaml_version.md)
* [set_yaml_version](set_yaml_version.md)