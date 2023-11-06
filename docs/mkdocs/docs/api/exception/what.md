<small>Defined in header [`<fkYAML/exception.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/exception.hpp)</small>

# <small>fkyaml::exception::</small>what

```cpp
const char* what();
```

Returns an error message for an exception. If nothing, a non-null, empty string will be returned.  

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

### **See Also**

* [(constructor)](constructor.md)
