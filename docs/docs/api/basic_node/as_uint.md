<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>as_uint

```cpp
uint64_t as_uint() const;
```

Returns the integer node value as an unsigned 64-bit integer.  
If the current node value is not a positive integer, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

## **Return Value**

the integer node value as an unsigned 64-bit integer.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/as_uint.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/as_uint.output"
    ```

## **See Also**

* [node_type](../node_type.md)