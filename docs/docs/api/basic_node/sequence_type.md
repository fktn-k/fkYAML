<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>sequence_type

```cpp
using sequence_type = SequenceType<basic_node>;
```

The type used to store sequence node values.  

The type is defined by the template parameter `SequenceType`.  
If not explicitly specified, the default type `std::vector` is defined.  
With the decided container type, the type of sequence objects will then be decided in the form of `SequenceType<basic_node>` with which sequence objects are stored inside a [`basic_node`](index.md).  

Note that sequence objects are stored as a pointer to an allocated memory area on the heap in a [`basic_node`](index.md) so that the internal storage size will at most be 8 bytes.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/sequence_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/sequence_type.output"
    ```

## **See Also**

* [basic_node](index.md)
