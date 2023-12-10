<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>add_anchor_name

```cpp
void add_anchor_name(const std::string& anchor_name);
void add_anchor_name(std::string&& anchor_name);
```

Adds an anchor name to the YAML node.  
If the basic_node has already had any anchor name, the new anchor name overwrites the old one.

## **Parameters**

***`anchor_name`*** [in]
:   An anchor name. This should not be empty.

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create a YAML node.
        fkyaml::node n = 123;

        // add an anchor name to the node.
        n.add_anchor_name("anchor");
        std::cout << n.get_anchor_name() << std::endl;

        // overwrite it with a new one.
        n.add_anchor_name("anchor2");
        std::cout << n.get_anchor_name() << std::endl;

        return 0;
    }
    ```

    ```bash
    anchor
    anchor2
    ```

## **See Also**

* [basic_node](index.md)
* [get_anchor_name](get_anchor_name.md)
