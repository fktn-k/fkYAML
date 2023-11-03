<small>Defined in header [`<fkYAML/exception.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/exception.hpp)</small>

# <small>fkyaml::exception::</small>(constructor)

```cpp
exception() = default;
```

Constructs an exception without an error message.  
You can specify an error message on constructing an exception with an overloaded constructor.  

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/exception.hpp>

    int main()
    {
        try
        {
            throw fkyaml::exception();
        }
        catch (const fkyaml::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
        return 0;
    }
    ```

    output:
    ```bash

    ```

## Overloads

```cpp
explicit exception(const char* msg);
```

Constructs an exception with a given error message.  
The given error message can be retrieved by calling [`exception::what()`](what.md) function.  

### **Parameters**

***`msg`*** [in]
:   An error message for the exception. If `nullptr` is given, the resulting error message will be empty.

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/exception.hpp>

    int main()
    {
        try
        {
            throw fkyaml::exception("An error message.");
        }
        catch (const fkyaml::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
        return 0;
    }
    ```

    output:
    ```bash
    An error message.
    ```