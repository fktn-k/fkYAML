<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>add_tag_name

```cpp
void add_tag_name(const std::string& tag_name);
void add_tag_name(std::string&& tag_name);
```

Adds a tag name to the YAML node.  
If the basic_node has already had any tag name, the new tag name overwrites the old one.

## **Parameters**

***`tag_name`*** [in]
:   A tag name. This should not be empty.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/add_tag_name.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/add_tag_name.output"
    ```

## **See Also**

* [basic_node](index.md)
* [get_tag_name](get_tag_name.md)
