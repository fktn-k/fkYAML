<small>Defined in header [`<fkYAML/exception.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/exception.hpp)</small>

# <small>fkyaml::</small>invalid_encoding

```cpp
class invalid_encoding : public exception;
```

A exception class indicating an encoding error.  
This class extends the [`fkyaml::exception`](index.md) class and the [`what()`](what.md) function emits an error message in the following format.  

```
invalid_encoding: [error message] in=[array of elements detected an error]
```

## **See Also**

* [exception](index.md)
* [what](what.md)
