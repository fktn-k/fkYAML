<small>Defined in header [`<fkYAML/exception.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/exception.hpp)</small>

# <small>fkyaml::</small>type_error

```cpp
class type_error : public exception;
```

A exception class indicating an invalid type conversion.  
This class extends the [`fkyaml::exception`](index.md) class and the [`what()`](what.md) function emits an error message in the following format.  

```
type_error: [error message] type=[source node type]
```

## **See Also**

* [exception](index.md)
* [what](what.md)
