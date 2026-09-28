<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>serialize

```cpp
static std::string serialize(const basic_node& node); // (1)
static void serialize(const basic_node& node, std::FILE* p_file); // (2)
static void serialize(const basic_node& node, std::ostream& os); // (3)
```

Serializes YAML node values recursively to a string, a file or an output stream.  
This function serializes the given `node` parameter in the following format.  

```yaml
# A scalar
{[anchor] [tag] value | alias}

# A sequence key presented as an explicit key node.
? - <sequence value>
  - <another sequence value>
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

Currently, there are some limitations:
* **Mappings and sequences are unconditionally emitted in the block styles.**  
  Even if an original YAML document contains flow style collection nodes, this function forces them to be emitted in the block style.  
* **Serialization unconditionally uses LFs (Unix Style) as the line break format.**  
  There is currently no way to change it to use CR+LFs (Windows Style) instead.  

## **Parameters**

***`node`*** [in]
:   A `basic_node` object to be serialized.

***`p_file`*** [in]
:   A pointer to a FILE object representing the output file.

***os*** [in]
:   An output stream representing the output destination.

## **Return Value**

The resulting string object of the serialization.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/serialize.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/serialize.output"
    ```

## **See Also**

* [basic_node](index.md)
* [add_anchor_name](add_anchor_name.md)
* [add_tag_name](add_tag_name.md)
* [deserialize](deserialize.md)
* [operator<<](insertion_operator.md)
* [operator"" _yaml](../operator_literal_yaml.md)
