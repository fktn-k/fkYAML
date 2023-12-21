<small>Defined in header [`<fkYAML/exception.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/exception.hpp)</small>

# <small>fkyaml::exception::</small>what

```cpp
const char* what() const noexcept;
```

Returns an error message for an exception. If nothing, a non-null, empty string will be returned.  

???+ Example

    ```cpp
    --8<-- "examples/ex_exception_what.cpp"
    ```

    output:
    ```bash
    --8<-- "examples/ex_exception_what.output"
    ```

### **See Also**

* [(constructor)](constructor.md)
