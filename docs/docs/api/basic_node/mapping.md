<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>mapping

```cpp
static basic_node mapping();
static basic_node mapping(const mapping_type& seq);
static basic_node mapping(mapping_type&& seq);
```

The factory method which constructs a basic_node with the [`node_type::MAPPING`](../node_type.md) type.  
Calling this function with no arguments will constructs a basic_node with an empty mapping node value.  
The resulting basic_node has the [`node_type::MAPPING`](../node_type.md) type.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/mapping.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/mapping.output"
    ```


## **See Also**

* [basic_node](index.md)
* [node_type](../node_type.md)
* [operator<<](insertion_operator.md)
