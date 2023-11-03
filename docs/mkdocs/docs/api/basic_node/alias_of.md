<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>alias_of

```cpp
static basic_node alias_of(const basic_node& anchor);
```

Creates an alias YAML node from an anchor node.  

## **Parameters**

***anchor*** [in]
:   A basic_node object with an anchor name.  
    This node must have some anchor name.  

## **Return Value**

An alias YAML node which is created from the given anchor node.  
If the given anchor node does not have any non-empty anchor name, an [`fkyaml::exception`](../exception/index.md) will be thrown.  

!!! Note

    If this API throws an exception, the internally stored YAML node value in the given anchor node stays intact.

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create a YAML node.
        fkyaml::node anchor_node = "test";

        // add an anchor name to the node.
        anchor_node.add_anchor_name("anchor");

        // create an alias YAML node.
        fkyaml::node alias_node = fkyaml::node::alias_of(anchor_node);

        // print the value in the alias node.
        std::cout<< alias_node.get_value_ref<fkyaml::node::string_type> << std::endl;

        return 0;
    }
    ```

    ```bash
    test
    ```

## **See Also**

* [basic_node](index.md)
* [add_anchor_name](add_anchor_name.md)
