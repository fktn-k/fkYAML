<small>Defined in header [`<fkYAML/exception.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/exception.hpp)</small>

# <small>fkyaml::</small>parse_error

```cpp
class parse_error : public exception;
```

A exception class indicating an error in parsing.  
This class extends the [`fkyaml::exception`](index.md) class and the [`what()`](what.md) function emits an error message in the following format.  

```
parse_error: [error message] (at line [LINE], column [COLUMN])
```

## **See Also**

* [exception](index.md)
* [what](what.md)
