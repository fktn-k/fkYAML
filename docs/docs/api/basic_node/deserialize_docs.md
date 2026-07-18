<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>deserialize_docs

```cpp
template <typename InputType>
static std::vector<basic_node> deserialize_docs(InputType&& input); // (1)

template <typename ItrType>
static std::vector<basic_node> deserialize_docs(ItrType&& begin, ItrType&& end); // (2)
```

Deserializes from compatible inputs.  
Unlike the [`deserialize()`](deserialize.md) function, this function deserializes all YAML documents in the input into [`fkyaml::basic_node`](index.md) objects.  
Prefer this function when your input may contain more than one YAML documents and all of them must be processed.  
Otherwise, use the [`deserialize()`](deserialize.md) function which is optimized for processing a single YAML document.  

Throws a [`fkyaml::exception`](../exception/index.md) if the deserialization process detects an error from the input.  

Since this function shares a large portion of internal implementation with the [`deserialize()`](deserialize.md) function, supported unicode encodings and line break formats are the same. Refer to its documentation for details.

## **Template Parameters**

***`InputType`***
:   the type of a compatible input, for instance:

    * an `std::istream` object
    * a `FILE` pointer (must not be `nullptr`)
    * a C-style array of characters (`char`, `char16_t` or `char32_t`. See the "Supported Unicode Encodings" above.)
        * char[N], char16_t[N], or char32_t[N] (N is the size of an array)
    * a container with which `begin(input)` and `end(input)` produces a valid pair of iterators
        * std::basic_string, std::array, std::string_view (with C++17 or better) and the likes.

***`ItrType`***
:   Type of a compatible iterator, for instance:

    * a pair of iterators which implement the [LegacyInputIterator](https://en.cppreference.com/w/cpp/named_req/InputIterator) requirements, e.g., return values of `std::string::begin()` and `std::string::begin()`
    * a pair of pointers such as `ptr` and `ptr + len`

## **Parameters**

***`input`*** [in]
:   An input source such as streams or arrays.

***`begin`*** [in]
:   An iterator to the first element of an input sequence

***`end`*** [in]
:   An iterator to the past-the-last element of an input sequence

## **Return Value**

The resulting `basic_node` objects of deserialization.

## **Examples**

??? Example "Example (a character array)"

    ```cpp
    --8<-- "apis/basic_node/deserialize_docs_char_array.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/deserialize_docs_char_array.output"
    ```

??? Example "Example (a std::string object)"

    ```cpp
    --8<-- "apis/basic_node/deserialize_docs_string.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/deserialize_docs_string.output"
    ```

??? Example "Example (a FILE pointer)"

    ```yaml title="input_multi.yaml"
    --8<-- "apis/input_multi.yaml"
    ```

    ```cpp
    --8<-- "apis/basic_node/deserialize_docs_file_pointer.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/deserialize_docs_file_pointer.output"
    ```

??? Example "Example (a pair of iterators)"

    ```cpp
    --8<-- "apis/basic_node/deserialize_docs_iterators.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/deserialize_docs_iterators.output"
    ```

## **See Also**

* [basic_node](index.md)
* [deserialize](deserialize.md)
* [get_value](get_value.md)
