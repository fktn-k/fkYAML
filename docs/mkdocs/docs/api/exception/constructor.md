<small>Defined in header [`<fkYAML/exception.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/exception.hpp)</small>

# <small>fkyaml::exception::</small>(constructor)

```cpp
exception() = default; // (1)

explicit exception(const char* msg) noexcept; // (2)
```

Constructs an exception object.   
You can specify an error message on constructing an exception with an overloaded constructor.  

## Overload (1)

```cpp
exception() = default; // (1)
```

Constructs an exception object without an error message.

???+ Example

    ```cpp
    --8<-- "examples/ex_exception_constructor_noarg.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_exception_constructor_noarg.output"
    ```

## Overloads (2)

```cpp
explicit exception(const char* msg) noexcept; // (2)
```

Constructs an exception with a given error message.  
The given error message can be retrieved by calling [`exception::what()`](what.md) function.  

### **Parameters**

***`msg`*** [in]
:   An error message for the exception. If `nullptr` is given, the resulting error message will be empty.

???+ Example

    ```cpp
    --8<-- "examples/ex_exception_constructor_msg.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_exception_constructor_msg.output"
    ```

## **See Also**

* [exception](index.md)
* [what](what.md)
