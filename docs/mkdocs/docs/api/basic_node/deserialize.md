<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>deserialize

```cpp
template <typename InputType>
static basic_node deserialize(InputType&& input); // (1)

template <typename ItrType>
static basic_node deserialize(ItrType&& begin, ItrType&& end); // (2)
```

Deserializes from compatible inputs.  
Note that this function deserializes only the first YAML document in the given input and ignore the rest.  
Use this function when the input consists of a single YAML document or only the first one needs to be deserialized.  
Otherwise, use the [`deserialize_docs()`](deserialize_docs.md) function instead.
Throws a [`fkyaml::exception`](../exception/index.md) if the deserialization process detects an error from the input.  

!!! note "Supported Unicode Encodings"

    fkYAML supports UTF-8, UTF-16 and UTF-32 encodings for input characters.  
    As the YAML specification shows [(here)](https://yaml.org/spec/1.2.2/#52-character-encodings), all input streams must begin with either a byte order mark(BOM) or an ASCII character, which will allow the encoding to be deduced by the pattern of the first few bytes of the input sequence.  
    If an input fails to meet the above requirement, the input is interpreted as a UTF-8 encoded character sequence starting without a BOM.  
    If a stream with `char` as a character type is used (including FILE pointers), the encoding will be automatically detected in the deserialization process, while an array/container of `char16_t` and `char32_t` denotes that its contents are encoded in the UTF-16BE/LE and UTF-32BE/LE format, respectively.  
    Furthermore, a byte order mark (BOM) can be put only at the beginning of an input sequence.  
    The deserialization process internally converts input characters into the UTF-8 encoded ones if they are encoded in the UTF-16 or UTF-32 format.  

!!! note "Supported newline codes"

    fkYAML supports LF (Unix style) and CR+LF (Windows style) as line break formats.  
    Inside the deserialization processes, however, fkYAML normalizes them into line feeds (LF, `0x0A`) just as described in the YAML specification (see the ["5.4. Line Break Characters"](https://yaml.org/spec/1.2.2/#54-line-break-characters) section).  
    Currently, there is no way to restore the original line break style in the serialization processes.  

## Overload (1)

```cpp
template <typename InputType>
static basic_node deserialize(InputType&& input);
```

### **Template Parameters**

***`InputType`***
:   Type of a compatible input, for instance:

    * an `std::istream` object
    * a `FILE` pointer (must not be `nullptr`)
    * a C-style array of characters (`char`, `char16_t` or `char32_t`. See the "Supported Unicode Encodings" above.)
        * char[N], char16_t[N], or char32_t[N] (N is the size of an array)
    * a container `obj` with which `begin(obj)` and `end(obj)` produces a valid pair of iterators
        * std::basic_string, std::array, std::string_view (with C++17 or better) and the likes.

### **Parameters**

***`input`*** [in]
:   An input source in the YAML format.

### **Return Value**

The resulting `basic_node` object deserialized from the input source.  

## Overload (2)

```cpp
template <typename ItrType>
static basic_node deserialize(ItrType&& begin, ItrType&& end);
```

### **Template Parameters**

***`ItrType`***
:   Type of a compatible iterator, for instance:

    * a pair of iterators such as return values of `std::string::begin()` and `std::string::end()`
    * a pair of pointers such as `ptr` and `ptr + len`

### **Parameters**

***`begin`*** [in]
:   An iterator to the first element of an input sequence

***`end`*** [in]
:   An iterator to the past-the-last element of an input sequence

### **Return Value**

The resulting `basic_node` object deserialized from the pair of iterators.

## Examples

???+ Example "Example (a character array)"

    ```cpp
    --8<-- "examples/ex_basic_node_deserialize_char_array.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_deserialize_char_array.output"
    ```

???+ Example "Example (a std::string object)"

    ```cpp
    --8<-- "examples/ex_basic_node_deserialize_string.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_deserialize_string.output"
    ```

???+ Example "Example (a FILE pointer)"

    ```yaml title="input.yaml"
    --8<-- "examples/input.yaml"
    ```

    ```cpp
    --8<-- "examples/ex_basic_node_deserialize_file_pointer.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_deserialize_file_pointer.output"
    ```

???+ Example "Example (a pair of iterators)"

    ```cpp
    --8<-- "examples/ex_basic_node_deserialize_iterators.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_deserialize_iterators.output"
    ```

### **See Also**

* [basic_node](index.md)
* [deserialize_docs](deserialize_docs.md)
* [get_value](get_value.md)
