<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::literals::yaml_literals::</small>operator"" _yaml

```cpp
inline node operator"" _yaml(const char* s, std::size_t n);
inline node operator"" _yaml(const char8_t* s, std::size_t n); // for C++20 or better
inline node operator"" _yaml(const char16_t* s, std::size_t n);
inline node operator"" _yaml(const char32_t* s, std::size_t n);
```

This operator implements a user-defined string literal for YAML node objects.  
It can be used by adding `#!cpp _yaml` to a string literal encoded in either UTF-8, UTF-16 or UTF-32 format and returns a [`node`](basic_node/node.md) object if no error occurred in the deserialization process.  
Calling this operator is equivalent with calling [`#!cpp fkyaml::basic_node::deserialize(s, s + n)`](basic_node/deserialize.md).

This operator is declared in the namespace `fkyaml::literals::yaml_literals`, where both `literals` and `yaml_literals` are [inline namespaces](https://en.cppreference.com/w/cpp/language/namespace#Inline_namespaces).  
Access to this operator can therefore be gained with one of the followings:  

* `#!cpp using namespace fkyaml::literals;`,
* `#!cpp using namespace fkyaml::yaml_literals;`
* `#!cpp using namespace fkyaml::literals::yaml_literals;`

## **Parameters**

***`s`*** [in]
:   A string representation of a YAML node object.  
    `char` or `char8_t`(for C++20 or better), `char16_t` and `char32_t` are interpreted as the UTF-8, UTF-16 and UTF-32 encodings, respectively.

***`n`*** [in]
:   The length of the string `s`.

## **Return Value**

The resulting basic_node object deserialized from the input string `s`.  

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/operator_literal_yaml/operator_literal_yaml.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/operator_literal_yaml/operator_literal_yaml.output"
    ```

## **See Also**

* [`node`](basic_node/node.md)
* [`basic_node::deserialize`](basic_node/deserialize.md)
* [`operator<<(basic_node)`](basic_node/insertion_operator.md)
