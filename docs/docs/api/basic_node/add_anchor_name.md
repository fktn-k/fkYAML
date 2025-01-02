<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>add_anchor_name

```cpp
void add_anchor_name(const std::string& anchor_name);
void add_anchor_name(std::string&& anchor_name);
```

Adds an anchor name to the YAML node.  
If the basic_node has already had any anchor name, the new anchor name overwrites the old one.

## **Parameters**

***`anchor_name`*** [in]
:   An anchor name. This should not be empty.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/add_anchor_name.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/add_anchor_name.output"
    ```

## **See Also**

* [basic_node](index.md)
* [get_anchor_name](get_anchor_name.md)
