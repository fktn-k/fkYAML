<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>mapping

```cpp
static basic_node mapping();
static basic_node mapping(const mapping_type& seq);
static basic_node mapping(mapping_type&& seq);
```

The factory method which constructs a basic_node with the [`node_t::MAPPING`](node_t.md) type from a mapping node value.  
Calling this API with no arguments will constructs a basic_node with an empty mapping node value.  
The resulting basic_node has the [`node_t::MAPPING`](node_t.md) type.  

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        fkyaml::node::mapping_type m = {
            {fkyaml::node(std::string("foo")), fkyaml::node(false)},
            {fkyaml::node(std::string("bar")), fkyaml::node(3.14)}
        };
        fkyaml::node n = fkyaml::node::mapping(m);
        std::cout << fkyaml::node::serialize(n) << std::endl;
        return 0;
    }
    ```
    output:

    ```yaml
    foo: false
    bar: 3.14
    ```


### **See Also**

* [basic_node](index.md)
* [node_t](node_t.md)
