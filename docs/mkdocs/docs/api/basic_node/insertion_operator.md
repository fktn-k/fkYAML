<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::</small>operator<<

```cpp
inline std::ostream& operator<<(std::ostream& os, const basic_node& n);
```

Extraction operator for basic_node template class.  
Serializes YAML node values into an output stream.  
This API is a wrapper of [`basic_node::serialize()`](serialize.md) function to simplify the implementation in the user's code.  
For more detailed descriptions, please visit the reference page for the [`basic_node::serialize()`](serialize.md) function.  

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

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create a basic_node object.
        fkyaml::node n = {
            {"foo", true},
            {"bar", {1, 2, 3}},
            {"baz", {
                {"qux", 3.14},
                {"corge", nullptr}
            }}
        };

        // serialize the basic_node object with insertion operator.
        // this is equivalent with:
        //   std::cout << fkyaml::node::serialize(n) << std::endl;
        std::cout << n << std::endl;

        return 0;
    }
    ```

    output:
    ```yaml
    foo: true
    bar:
      - 1
      - 2
      - 3
    baz:
      qux: 3.14
      corge: null
    ```

### **See Also**

* [basic_node](index.md)
* [serialize](serialize.md)
* [deserialize](deserialize.md)
* [operator>>](extraction_operator.md)
