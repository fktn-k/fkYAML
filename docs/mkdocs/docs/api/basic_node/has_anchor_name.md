<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>has_anchor_name

```cpp
bool has_anchor_name() const noexcept;
```

Check if the YAML node has an anchor name.  

## **Return Value**

`true` if the YAML node has an anchor name, `false` otherwise.  

???+ Example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // create a YAML node.
        fkyaml::node n = {{"foo", true}};

        // check if the node has an anchor name.
        std::cout << std::boolalpha << n.has_anchor_name() << std::endl;

        // set an anchor name.
        n.add_anchor_name("anchor");

        // check if the node has an anchor name again.
        std::cout << std::boolalpha << n.has_anchor_name() << std::endl;

        return 0;
    }
    ```

    output:
    ```bash
    false
    true
    false
    ```

## **See Also**

* [basic_node](index.md)
* [add_anchor_name](add_anchor_name.md)
