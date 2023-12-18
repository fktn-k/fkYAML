<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_anchor_name

```cpp
const std::string& get_anchor_name() const;
```

Gets the anchor name associated to the YAML node.  
Some anchor name must be set before calling this API.  
Calling [`has_anchor_name`](has_anchor_name.md) to see if the node has any anchor name beforehand.

## **Return Value**

The anchor name associated to the node.  
If no anchor name has been set, an [`fkyaml::exception`](../exception/index.md) will be thrown.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_get_anchor_name.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_get_anchor_name.output"
    ```

## **See Also**

* [basic_node](index.md)
* [set_anchor_name](get_anchor_name.md)
