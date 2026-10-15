<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_location

```cpp
struct node_location {
    uint32_t line;
    uint32_t column;
};

node_location get_location() const noexcept;
```

Gets the location in the deserialized input where the YAML node begins.  
This is useful to report errors found while validating deserialized nodes, for example.  
Both the line and the column are 0-based, which is consistent with [`fkyaml::parse_error`](../exception/parse_error.md).  

This function and the `fkyaml::node_location` type are available only if `FK_YAML_ENABLE_NODE_LOCATION` is defined before including the header files of the library. See [Macros](../macros.md#node-location) for more details.  

A node begins at:

| Node                                   | Location                                    |
| -------------------------------------- | ------------------------------------------- |
| a scalar or an alias                   | its first character                         |
| a block sequence                       | its first `-` indicator                     |
| a block mapping                        | its first key                               |
| a flow sequence/mapping                | its `[`/`{` indicator                       |
| an empty mapping value                 | its key                                     |
| an empty block sequence entry          | its `-` indicator                           |
| a node not created by deserialization  | line 0, column 0                            |

Node properties (tags and anchors) are not a part of the location.  
The location is a position in the input after it is converted into UTF-8 with line breaks normalized, so the column counts bytes in UTF-8.  

## **Return Value**

The location where the node begins.  

## **Examples**

??? Example

    ```cpp
    #define FK_YAML_ENABLE_NODE_LOCATION
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main() {
        fkyaml::node root = fkyaml::node::deserialize("server:\n  port: 70000\n");

        const fkyaml::node& port = root["server"]["port"];
        if (port.get_value<int>() > 65535) {
            const fkyaml::node_location loc = port.get_location();
            std::cout << "error at line " << loc.line + 1 << ", column " << loc.column + 1
                      << ": port must be between 1 and 65535" << std::endl;
        }
        return 0;
    }
    ```

    output:
    ```bash
    error at line 2, column 9: port must be between 1 and 65535
    ```

## **See Also**

* [basic_node](index.md)
* [deserialize](deserialize.md)
* [Macros](../macros.md)
