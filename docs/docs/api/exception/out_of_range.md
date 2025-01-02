<small>Defined in header [`<fkYAML/exception.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/exception.hpp)</small>

# <small>fkyaml::</small>out_of_range

```cpp
class out_of_range : public exception;
```

A exception class indicating an invalid type conversion.  
This class extends the [`fkyaml::exception`](index.md) class and the [`what()`](what.md) function emits an error message in either of the following formats:

* if an index which is used to access an element is out of range.
    ```
    out_of_range: index [index value] is out of range.
    ```
* if a key which is used to access an element is not found.
    ```
    out_of_range: key '[key name]' is not found.
    ```

## **See Also**

* [exception](index.md)
* [what](what.md)
