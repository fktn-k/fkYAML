<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>as_map

```cpp
mapping_type& as_map();
const mapping_type& as_map() const;
```

Returns (const) reference to the mapping node value.  
If the current node value is not a mapping, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

## **Return Value**

(const) reference to the mapping node value.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/as_map.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/as_map.output"
    ```

## **See Also**

* [node_type](../node_type.md)
