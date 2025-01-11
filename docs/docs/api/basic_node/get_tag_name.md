<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_tag_name

```cpp
const std::string& get_tag_name() const;
```

Gets a tag name associated to the YAML node.  
Some tag name must be set before calling this API.  
Call [`has_tag_name`](has_tag_name.md) to see if the node has any tag name beforehand.  
If no tag name has been set, an [`fkyaml::exception`](../exception/index.md) will be thrown.

## **Return Value**

The tag name associated to the node.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/get_tag_name.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/get_tag_name.output"
    ```

## **See Also**

* [basic_node](index.md)
* [has_tag_name](has_tag_name.md)
