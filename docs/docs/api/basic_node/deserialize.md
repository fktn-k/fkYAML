<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>deserialize

```cpp
template <typename InputType>
static basic_node deserialize(InputType&& input); // (1)

template <typename ItrType>
static basic_node deserialize(ItrType begin, ItrType end); // (2)
```

Deserializes from compatible inputs.  
Note that this function deserializes only the first YAML document in the given input and ignore the rest.  
Use this function when the input consists of a single YAML document or only the first needs to be deserialized.  
Otherwise, use the [`deserialize_docs()`](deserialize_docs.md) function instead.
Throws a [`fkyaml::exception`](../exception/index.md) if the deserialization process detects an error from the input.  

!!! note "Supported Unicode Encodings"

    fkYAML supports UTF-8, UTF-16 and UTF-32 encodings for input characters.  
    As the YAML specification shows [here](https://yaml.org/spec/1.2.2/#52-character-encodings), all input streams must begin with either a byte order mark(BOM) or an ASCII character, which will allow the encoding to be deduced by the pattern of the first few bytes.  
    If an input does not start either of them, the input is considered to be encoded in the UTF-8 encoding starting with no BOM.  
    Furthermore, allowed encoding types differ depending on the type of input characters. If a deduced encoding type is not allowed for the type, a [fkyaml::exception] will be thrown.

    | type                                                                         | allowed encodings     |
    | ---------------------------------------------------------------------------- | --------------------- |
    | [`char`](https://en.cppreference.com/w/cpp/keyword/char)                     | UTF-8, UTF-16, UTF-32 |
    | [`char8_t`](https://en.cppreference.com/w/cpp/keyword/char8_t) (since C++20) | UTF-8                 |
    | [`char16_t`](https://en.cppreference.com/w/cpp/keyword/char16_t)             | UTF-16                |
    | [`char32_t`](https://en.cppreference.com/w/cpp/keyword/char32_t)             | UTF-32                |

!!! note "Supported line break formats"

    fkYAML supports LF (Unix style) and CR+LF (Windows style) as line break formats.  
    Inside the deserialization processes, fkYAML normalizes them into line feeds (LF, `0x0A`).  
    In other words, if an input uses the Windows style as its line break format, there happens a copy of the input contents. So, the Unix style is recommended for better performance.  
    Currently, there is no way to restore the original line break style.  

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

The resulting `basic_node` object of deserialization.

## **Examples**

??? Example "Example (a character array)"

    ```cpp
    --8<-- "apis/basic_node/deserialize_char_array.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/deserialize_char_array.output"
    ```

??? Example "Example (a std::string object)"

    ```cpp
    --8<-- "apis/basic_node/deserialize_string.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/deserialize_string.output"
    ```

??? Example "Example (a FILE pointer)"

    ```yaml title="input.yaml"
    --8<-- "apis/input.yaml"
    ```

    ```cpp
    --8<-- "apis/basic_node/deserialize_file_pointer.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/deserialize_file_pointer.output"
    ```

??? Example "Example (a pair of iterators)"

    ```cpp
    --8<-- "apis/basic_node/deserialize_iterators.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/deserialize_iterators.output"
    ```

## **See Also**

* [basic_node](index.md)
* [deserialize_docs](deserialize_docs.md)
* [get_value](get_value.md)
