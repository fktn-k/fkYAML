<small>Defined in header [`<fkYAML/exception.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/exception.hpp)</small>

# <small>fkyaml::exception::</small>(constructor)

```cpp
exception() = default; // (1)

explicit exception(const char* msg) noexcept; // (2)
```

Constructs an exception object.   
You can specify an error message on constructing an object with an overloaded constructor.  

## **Parameters**

***`msg`*** [in]
:   An error message for the exception. If `nullptr` is given, the resulting error message will be empty.

??? Example "Overload(1): create a default value"

    ```cpp
    --8<-- "apis/exception/constructor_noarg.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/exception/constructor_noarg.output"
    ```

## **Examples**

??? Example "Overload(2): create an object with an error message"

    ```cpp
    --8<-- "apis/exception/constructor_msg.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/exception/constructor_msg.output"
    ```

## **See Also**

* [exception](index.md)
* [what](what.md)
