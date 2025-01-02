<small>Defined in header [`<fkYAML/exception.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/exception.hpp)</small>

# <small>fkyaml::exception::</small>what

```cpp
const char* what() const noexcept;
```

Returns an error message for an exception. If nothing, a non-null pointer to an empty string will be returned.  

## **Return Value**

An error message for an exception or a non-null pointer to an empty string.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/exception/what.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/exception/what.output"
    ```

## **See Also**

* [(constructor)](constructor.md)
