<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>serialize

```cpp
static std::string serialize(const basic_node& node);
```

Serializes YAML node values recursively.  
Currently, the serialization of mappings and sequences only supports block styles.  
That means that, even if a deserialized source input is originally written in flow styles, the serialization result forces to convert it in block styles.   
This function serializes the given `node` parameter in the following format.  
```yaml
# A scalar
{[anchor] [tag] value | alias}

# A sequence key presented as an explicit key node.
? - <sequence value>
  - <another sequnce value>
: <scalar>

# A sequence value.
# Extra 2 spaces are inserted before the sequence indicators denoted as "- ".
# If an anchor and/or a tag are set to the sequence, they are put on the same line as the key.
<scalar>: [anchor] [tag]
  - <sequence scalar value>
  - [anchor] [tag] # A child sequence node.
    - <child sequence value>
  - [anchor] [tag] # A mapping node.
    <mapping>

# A mapping whose value is a scalar.
<scalar>: <scalar>

# A mapping key presented as an explicit key node.
? <mapping>
: <scalar>

# A mapping value whose value is a container node, either a mapping or a sequence.
# Extra 2 spaces are inserted before the value to indicate indentation.
# If an anchor and/or a tag are set to the mapping, they are put on the same line as the key.
<scalar>: [anchor] [tag]
  <mapping scalar key>: <mapping scalar value>
  <mapping scalar key>: [anchor] [tag] # A child mapping node.
    <child mapping scalar key>: <child mapping scalar value>
```

### **Parameters**

***`node`*** [in]
:   A `basic_node` object to be serialized.

### **Return Value**

The resulting string object from the serialization of the `node` object.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_serialize.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_serialize.output"
    ```

### **See Also**

* [basic_node](index.md)
* [add_anchor_name](add_anchor_name.md)
* [add_tag_name](add_tag_name.md)
* [deserialize](deserialize.md)
* [operator<<](insertion_operator.md)
* [operator"" _yaml](../operator_literal_yaml.md)
