<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>as_bool

```cpp
boolean_type& as_bool();
const boolean_type& as_bool() const;
```

Returns (const) reference to the boolean node value.  
If the current node value is not a boolean, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

## **Return Value**

(const) reference to the boolean node value.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/as_bool.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/as_bool.output"
    ```

## **See Also**

* [node_type](../node_type.md)
