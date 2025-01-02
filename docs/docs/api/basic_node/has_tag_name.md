<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>has_tag_name

```cpp
bool has_tag_name() const noexcept;
```

Check if the node has a tag name.  

## **Return Value**

`true` if the node has a tag name, `false` otherwise.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/has_tag_name.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/has_tag_name.output"
    ```

## **See Also**

* [basic_node](index.md)
* [add_tag_name](add_tag_name.md)
