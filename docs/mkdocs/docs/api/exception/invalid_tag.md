<small>Defined in header [`<fkYAML/exception.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/exception.hpp)</small>

# <small>fkyaml::</small>invalid_tag

```cpp
class invalid_tag : public exception;
```

A exception class indicating an invalid tag.  
This class extends the [`fkyaml::exception`](index.md) class and the [`what()`](what.md) function emits an error message in the following format.  

```
invalid_tag: [error message] tag=[invalid tag contents]
```

## **See Also**

* [exception](index.md)
* [what](what.md)
