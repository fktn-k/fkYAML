<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>sequence

```cpp
static basic_node sequence();
static basic_node sequence(const sequence_type& seq);
static basic_node sequence(sequence_type&& seq);
```

The factory method which constructs a basic_node with the [`node_t::SEQUENCE`](node_t.md) type from a sequence node value.  
Calling this API with no arguments will constructs a basic_node with an empty sequence node value.  
The resulting basic_node has the [`node_t::SEQUENCE`](node_t.md) type.  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_sequence.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_sequence.output"
    ```


### **See Also**

* [basic_node](index.md)
* [node_t](node_t.md)
* [operator<<](insertion_operator.md)
