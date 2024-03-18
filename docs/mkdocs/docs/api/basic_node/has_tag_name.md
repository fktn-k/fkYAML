<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>has_tag_name

```cpp
bool has_tag_name() const noexcept;
```

Check if the YAML node has a tag name.  

## **Return Value**

`true` if the YAML node has a tag name, `false` otherwise.  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_has_tag_name.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_has_tag_name.output"
    ```

## **See Also**

* [basic_node](index.md)
* [add_tag_name](add_tag_name.md)
