<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>serialize_docs

```cpp
static std::string serialize_docs(const std::vector<basic_node>& docs);
```

Serializes YAML documents into a string.  
This function serializes the given `docs` parameter with the separation line (...) between YAML documents.  
Regarding the serialization of each document, see the documentation for the [`serialize()`](serialize.md) function with which this function shares internal implementation.  
Just as the [`serialize()`](serialize.md) function does, fkYAML unconditionally uses LFs as the line break format in serialization outputs, and there is currently no way to change it to use CR+LFs instead.  

```yaml
<YAML Document 1>
...
<YAML Document 2>
# the last document end marker (...) is omitted since it's redundant.
```

## **Parameters**

***`docs`*** [in]
:   `basic_node` objects to be serialized.

## **Return Value**

The resulting string object of the serialization.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/serialize_docs.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/serialize_docs.output"
    ```

## **See Also**

* [basic_node](index.md)
* [add_anchor_name](add_anchor_name.md)
* [add_tag_name](add_tag_name.md)
* [deserialize](deserialize.md)
* [operator<<](insertion_operator.md)
* [operator"" _yaml](../operator_literal_yaml.md)
