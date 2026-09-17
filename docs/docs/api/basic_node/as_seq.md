<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>as_seq

```cpp
sequence_type& as_seq();
const sequence_type& as_seq() const;
```

Returns (const) reference to the sequence node value.  
If the current node value is not an sequence, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

## **Return Value**

(const) reference to the sequence node value.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/as_seq.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/as_seq.output"
    ```

## **See Also**

* [node_type](../node_type.md)
