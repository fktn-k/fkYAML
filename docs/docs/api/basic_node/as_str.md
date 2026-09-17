<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>as_str

```cpp
string_type& as_str();
const string_type& as_str() const;
```

Returns (const) reference to the string node value.  
If the current node value is not an string, a [`fkyaml::type_error`](../exception/type_error.md) will be thrown.  

## **Return Value**

(const) reference to the string node value.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/as_str.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/as_str.output"
    ```

## **See Also**

* [node_type](../node_type.md)
