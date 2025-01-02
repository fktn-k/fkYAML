<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>boolean_type

```cpp
using boolean_type = BooleanType;
```

The type used to store boolean values.  
The YAML specification describes a boolean as a type which differentiates the following literals:

* true, True, TRUE -> `true`
* false, False, FALSE -> `false`

The actual type is defined by the template parameter `BooleanType`.  
If not explicitly specified, the default type `bool` is defined.  
With the decided type, boolean values are stored directly inside a [`basic_node`](index.md).  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/boolean_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/boolean_type.output"
    ```

### **See Also**

* [basic_node](index.md)
