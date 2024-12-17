<small>Defined in header [`<fkYAML/exception.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/exception.hpp)</small>

# <small>fkyaml::</small>exception

```cpp
class exception : public std::exception;
```

A basic exception class used in the fkYAML library.

## **Derived Classes**

| Type                                    | Description                                          |
| --------------------------------------- | ---------------------------------------------------- |
| [invalid_encoding](invalid_encoding.md) | The exception indicating an encoding error.          |
| [invalid_tag](invalid_tag.md)           | The exception indicating an invalid tag.             |
| [out_of_range](out_of_range.md)         | The exception indicating an out-of-range error.      |
| [parse_error](parse_error.md)           | The exception indicating an error in parsing.        |
| [type_error](type_error.md)             | The exception indicating an invalid type conversion. |

## **Member Functions**

### Construction/Destruction

| Name                            | Description              |
| ------------------------------- | ------------------------ |
| [(constructor)](constructor.md) | constructs an exception. |
| [(destructor)](destructor.md)   | destroys an exception.   |

### Operation

| Name            | Description                                |
| --------------- | ------------------------------------------ |
| [what](what.md) | provides an error message for a exception. |
