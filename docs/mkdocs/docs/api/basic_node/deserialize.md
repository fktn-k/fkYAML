<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>deserialize

```cpp
template <typename InputType>
static basic_node deserialize(InputType&& input); // (1)

template <typename ItrType>
static basic_node deserialize(ItrType&& begin, ItrType&& end); // (2)

template <typename PtrType, detail::enable_if_t<std::is_pointer<PtrType>::value, int> = 0>
static basic_node deserialize(PtrType&& ptr, std::size_t size); // (3)
```

Deserializes from compatible input sources.  
Throws a [`fkyaml::exception`](../exception/index.md) if the deserialization process detects an error from the input sources.  

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
    * a C-style array of characters
    * a pointer to a null-terminated string of single byte characters.
    * a container `obj` for which `begin(obj)` and `end(obj)` produces a valid pair of iterators
        * std::string, std::array<char>, and the likes.

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

    * a pair of `std::string::iterator`
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
    #include <cstdint>
    #include <iomanip>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // deserialize a YAML string.
        char input[] = R"(
        foo: true
        bar: 123
        baz: 3.14
        )";
        fkyaml::node n = fkyaml::node::deserialize(input);

        // check the deserialization result.
        std::cout << n["foo"].get_value<bool>() << std::endl;
        std::cout << n["bar"].get_value<std::int64_t>() << std::endl;
        std::cout << std::setprecision(3) << n["baz"].get_value<double>() << std::endl;

        return 0;
    }
    ```

    ```bash
    true
    123
    3.14
    ```

???+ Example "Example (a std::string object)"

    ```cpp
    #include <cstdint>
    #include <iomanip>
    #include <iostream>
    #include <string>
    #include <fkYAML/node.hpp>

    int main()
    {
        // deserialize a YAML string.
        std::string s = R"(
        foo: true
        bar: 123
        baz: 3.14
        )";
        fkyaml::node n = fkyaml::node::deserialize(s);

        // check the deserialization result.
        std::cout << n["foo"].get_value<bool>() << std::endl;
        std::cout << n["bar"].get_value<std::int64_t>() << std::endl;
        std::cout << std::setprecision(3) << n["baz"].get_value<double>() << std::endl;

        return 0;
    }
    ```

    ```bash
    true
    123
    3.14
    ```

???+ Example "Example (a FILE pointer)"

    ```yaml title="input.yaml"
    foo: true
    bar: 123
    baz: 3.14
    ```

    ```cpp
    #include <cstdint>
    #include <iomanip>
    #include <iostream>
    #include <string>
    #include <fkYAML/node.hpp>

    int main()
    {
        // deserialize a YAML string.
        FILE* p_file = std::fopen("input.yaml", "r");
        if (!p_file)
        {
            // You must not pass a null FILE pointer.
            return -1;
        }
        fkyaml::node n = fkyaml::node::deserialize(p_file);

        // check the deserialization result.
        std::cout << n["foo"].get_value<bool>() << std::endl;
        std::cout << n["bar"].get_value<std::int64_t>() << std::endl;
        std::cout << std::setprecision(3) << n["baz"].get_value<double>() << std::endl;

        return 0;
    }
    ```

    ```bash
    true
    123
    3.14
    ```

???+ Example "Example (a pair of iterators)"

    ```cpp
    #include <cstdint>
    #include <iomanip>
    #include <iostream>
    #include <string>
    #include <fkYAML/node.hpp>

    int main()
    {
        // deserialize a YAML string.
        std::array<char> input = {
            'f', 'o', 'o', ':', ' ', 't', 'r', 'u', 'e', 'd', 'u', 'm', 'm', 'y'
        };
        fkyaml::node n = fkyaml::node::deserialize(input.begin(), input.begin() + 9);

        // check the deserialization result.
        std::cout << n["foo"].get_value<bool>() << std::endl;
        std::cout << n["bar"].get_value<std::int64_t>() << std::endl;
        std::cout << std::setprecision(3) << n["baz"].get_value<double>() << std::endl;

        return 0;
    }
    ```

    ```bash
    true
    123
    3.14
    ```

### **See Also**

* [basic_node](index.md)
* [get_value](get_value.md)
