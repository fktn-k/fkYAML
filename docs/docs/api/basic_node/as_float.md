<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>as_float

```cpp
float_number_type& as_float();
const float_number_type& as_float() const;
```

Returns (const) reference to the float node value.  
If the current node value is not a floating point value, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

## **Return Value**

(const) reference to the float node value.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/as_float.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/as_float.output"
    ```

## **See Also**

* [node_type](../node_type.md)
