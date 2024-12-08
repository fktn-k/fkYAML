<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>reverse_iterator, <small>fkyaml::basic_node::</small>const_reverse_iterator

```cpp
using reverse_iterator = detail::reverse_iterator<basic_node>;
using const_reverse_iterator = detail::reverse_iterator<const basic_node>;
```

The types of non-const/const reverse iterators for [`basic_node`](index.md) containers.  
They satisfies [LegacyBidirectionalIterator](https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator) and are commonly used for sequence and mapping container nodes.  

## Member Types

### reverse_iterator
| Type                | Definition                                                                                    |
| ------------------- | --------------------------------------------------------------------------------------------- |
| `iterator_type`     | `basic_node::iterator`                                                                                  |
| `value_type`        | `basic_node`                                                                                  |
| `reference`         | `basic_node&`                                                                                 |
| `pointer`           | `basic_node*`                                                                                 |
| `difference_type`   | [`std::ptrdiff_t`](https://en.cppreference.com/w/cpp/types/ptrdiff_t)                         |
| `iterator_category` | [`std::bidirectional_iterator_tag`](https://en.cppreference.com/w/cpp/iterator/iterator_tags) |

### const_reverse_iterator
| Type                | Definition                                                                                    |
| ------------------- | --------------------------------------------------------------------------------------------- |
| `iterator_type`     | `basic_node::const_iterator`                                                                                  |
| `value_type`        | `basic_node`                                                                                  |
| `reference`         | `const basic_node&`                                                                           |
| `pointer`           | `const basic_node*`                                                                           |
| `difference_type`   | [`std::ptrdiff_t`](https://en.cppreference.com/w/cpp/types/ptrdiff_t)                         |
| `iterator_category` | [`std::bidirectional_iterator_tag`](https://en.cppreference.com/w/cpp/iterator/iterator_tags) |

## Member Functions

| Name  | Description                                                                                                                                        |
| ----- | -------------------------------------------------------------------------------------------------------------------------------------------------- |
| key   | Returns const reference to a key node of the current key-value pair.<br>`fkyaml::exception` is thrown if an iterator points to a sequence element. |
| value | Returns reference to a sequence element if an element points to a sequence element, or a mapping value otherwise.                                  |

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_reverse_iterator.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_reverse_iterator.output"
    ```

### **See Also**

* [basic_node](index.md)
* [iterator, const_iterator](iterator.md)
* [rbegin, crbegin](rbegin.md)
* [rend, crend](rend.md)
* [operator<<](insertion_operator.md)
