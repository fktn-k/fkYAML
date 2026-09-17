<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_resolved_tag_name

```cpp
std::string get_resolved_tag_name() const;
```

Gets a resolved tag name associated to the YAML node.  
Some tag name must be set before calling this API.  
Call [`has_tag_name`](has_tag_name.md) to see if the node has any tag name beforehand.  
If no tag name has been set, an [`fkyaml::exception`](../exception/index.md) will be thrown.  

If the target node has a tag `!!str`, the returned value would be `tag:yaml.org,2002:str`.  
This is because `!!` is the secondary tag handle and resolved to `tag:yaml.org,2002:` by default.  
See https://yaml.org/spec/1.2.2/#6821-tag-handles for more details on the tag handles.  

If the target node has a verbatim tag `!<...>`, this method returns the `...` part  
as the YAML specification states at https://yaml.org/spec/1.2.2/#691-node-tags.

## **Return Value**

The tag name associated to the node.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/get_resolved_tag_name.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/get_resolved_tag_name.output"
    ```

## **See Also**

* [basic_node](index.md)
* [has_tag_name](has_tag_name.md)
