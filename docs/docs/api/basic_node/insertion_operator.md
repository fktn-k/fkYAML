<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::</small>operator<<

```cpp
inline std::ostream& operator<<(std::ostream& os, const basic_node& n);
```

Serializes a node value into an output stream.  
This operator is a wrapper of the [`basic_node::serialize()`](serialize.md) function to simplify the implementation in the client code.  
For details, please visit the documentation of the [`serialize()`](serialize.md) function.  

## **Template Parameters**

***SequenceType***
:   Type for sequence node value containers.

***MappingType***
:   Type for mapping node value containers.

***BooleanType***
:   Type for boolean node values.

***IntegerType***
:   Type for integer node values.

***FloatNumberType***
:   Type for float number node values.

***StringType***
:   Type for string node values.

***ConverterType***
:   Type for converters between nodes and values of native data types.

## **Parameters**

***`os`*** [in]
:   An output stream object.

***`n`*** [in]
:   A basic_node object.

## **Return Value**

Reference to the output stream object `os`.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/insertion_operator.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/insertion_operator.output"
    ```

### **See Also**

* [basic_node](index.md)
* [serialize](serialize.md)
* [deserialize](deserialize.md)
* [operator>>](extraction_operator.md)
