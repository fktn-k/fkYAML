<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>as_int

```cpp
integer_type& as_int();
const integer_type& as_int() const;
```

Returns (const) reference to the integer node value.  
If the current node value is not an integer, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

## **Return Value**

(const) reference to the integer node value.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/as_int.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/as_int.output"
    ```

## **See Also**

* [node_type](../node_type.md)
