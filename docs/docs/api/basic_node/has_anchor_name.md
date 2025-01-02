<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>has_anchor_name

```cpp
bool has_anchor_name() const noexcept;
```

Check if the node has an anchor name.  

## **Return Value**

`true` if the node has an anchor name, `false` otherwise.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/has_anchor_name.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/has_anchor_name.output"
    ```

## **See Also**

* [basic_node](index.md)
* [add_anchor_name](add_anchor_name.md)
