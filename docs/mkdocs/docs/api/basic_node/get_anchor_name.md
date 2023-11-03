<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>get_anchor_name

```cpp
const std::string& get_anchor_name() const;
```

Gets the anchor name associated to the YAML node.  
Some anchor name must be set before calling this API.  
Calling [`has_anchor_name`](has_anchor_name.md) to see if the node has any anchor name beforehand.

## **Return Value**

The anchor name associated to the node.  
If no anchor name has been set, an [`fkyaml::exception`](../exception/index.md) will be thrown.

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create a YAML node.
        fkyaml::node n = 123;

        // try to get an anchor name before any anchor name has been set.
        try
        {
            std::cout << n.get_anchor_name() << std::endl;
        }
        catch (const fkyaml::exception& e)
        {
            std::cout << e.what() << std::endl;
        }

        // add an anchor name to the node.
        n.add_anchor_name("anchor");
        std::cout << n.get_anchor_name() << std::endl;

        return 0;
    }
    ```

    output:
    ```bash
    No anchor name has been set.
    anchor
    ```

## **See Also**

* [basic_node](index.md)
* [set_anchor_name](get_anchor_name.md)
