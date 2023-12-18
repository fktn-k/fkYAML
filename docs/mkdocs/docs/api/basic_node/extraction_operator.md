<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::</small>operator>>

```cpp
inline std::istream& operator>>(std::istream& is, basic_node& n);
```

Insertion operator for basic_node template class.  
Deserializes an input stream into a [`basic_node`](index.md).  
This API is a wrapper of [`basic_node::deserialize()`](deserialize.md) function for input streams to simplify the implementation in the user's code.  
Note that the contents of the input stream must be encoded in either the UTF-8, UTF-16BE/LE or UTF-32LE/BE format and begin with either a byte order mark or an ASCII character.  

## **Parameters**

***`is`*** [in]
:   An input stream object.

***`n`*** [in]
:   A basic_node object.

## **Return Value**

Reference to the input stream object `is`.  

???+ Example

    ```yaml
    --8<-- "examples/input.yaml"
    ```

    ```cpp
    --8<-- "examples/ex_basic_node_extraction_operator.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_extraction_operator.output"
    ```

### **See Also**

* [basic_node](index.md)
* [deserialize](deserialize.md)
* [serialize](serialize.md)
* [operator<<](insertion_operator.md)
