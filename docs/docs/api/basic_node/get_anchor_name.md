<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_anchor_name

```cpp
const std::string& get_anchor_name() const;
```

Gets an anchor name associated with the YAML node.  
Some anchor name must be set before calling this function.  
Call [`has_anchor_name`](has_anchor_name.md) to see if the node has any anchor name beforehand.  
If no anchor name has been set, an [`fkyaml::exception`](../exception/index.md) will be thrown.

## **Return Value**

The anchor name associated to the node.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/get_anchor_name.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/get_anchor_name.output"
    ```

## **See Also**

* [basic_node](index.md)
* [has_anchor_name](has_anchor_name.md)
