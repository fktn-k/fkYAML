<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::</small>node

```cpp
using node = basic_node<>;
```

This type is the default specialization of the [basic_node](index.md) class which uses the standard template types.  

??? Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create a YAML node.
        fkyaml::node n =
        {
            {std::string("foo"), 3.14},
            {std::string("bar"), true},
            {std::string("baz"), nullptr},
            {std::string("qux"), {
                {std::string("corge"), {1, 2, 3}}
            }}
        };

        // add a new value.
        n["qux"]["key"] = {std::string("another"), std::string("value")};

        // output a YAML formatted string.
        std::cout << fkyaml::node::serialize(n) << std::endl;
    };
    ```

    output:
    ```yaml
    bar: true
    baz: null
    foo: 3.14
    qux:
      corge:
        - 1
        - 2
        - 3
      key:
        another: value
    ```

### **See Also**

* [basic_node](index.md)