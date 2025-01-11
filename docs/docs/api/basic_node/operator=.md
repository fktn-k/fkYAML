<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>operator=

```cpp
basic_node& operator=(const basic_node& rhs) noexcept; // (1)

basic_node& operator=(basic_node&& rhs) noexcept; // (2)
```

Assignment operator.  
The `basic_node` class supports both copy(1) and move(2) assignments.  

### **Parameters**

***`rhs`*** [in]
:   A basic_node to copy/move from.

### **Return Value**

Reference to `this` basic_node object.

??? Example "Overload(1): copy assignments a basic_node"

    ```cpp
    --8<-- "apis/basic_node/copy_assignment_operator.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/copy_assignment_operator.output"
    ```

## **Examples**

??? Example "Overload(2): move assignments a basic_node"

    ```cpp
    --8<-- "apis/basic_node/move_assignment_operator.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/move_assignment_operator.output"
    ```

## **See Also**

* [basic_node](index.md)
* [is_integer](is_integer.md)
* [get_value](get_value.md)
