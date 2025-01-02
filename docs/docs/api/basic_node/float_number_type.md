<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>float_number_type

```cpp
using float_number_type = FloatNumberType;
```

The type used to store floating point values.  
The YAML specification describes a floating point value as a type which differentiates the following literals:

* real numbers (with scientific notation) → a floating point value
* (+/-).inf, (+/-).Inf, (+/-).INF → the positive/negative infinity
* .nan, .NaN, .NAN → Not a Number

The actual type is defined by the template parameter `FloatNumberType`.  
If not explicitly specified, the default type `double` is defined.  
With the decided type, floating point values are stored directly inside a [`basic_node`](index.md).  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/float_number_type.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/float_number_type.output"
    ```

## **See Also**

* [basic_node](index.md)
