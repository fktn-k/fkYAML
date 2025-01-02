<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>alias_of

```cpp
static basic_node alias_of(const basic_node& anchor);
```

Creates an alias YAML node from an anchor node.  
If the given anchor node does not have any non-empty anchor name, an [`fkyaml::exception`](../exception/index.md) will be thrown.  

## **Parameters**

***anchor*** [in]
:   A basic_node object with an anchor name.  
    This node must have a non-empty anchor name.  

## **Return Value**

An alias node which refers to the given anchor node.  

!!! Note

    If this API throws an exception, the internally stored YAML node value in the given anchor node stays intact.

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/alias_of.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/alias_of.output"
    ```

## **See Also**

* [basic_node](index.md)
* [add_anchor_name](add_anchor_name.md)
