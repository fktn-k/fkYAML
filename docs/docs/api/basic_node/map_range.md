<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>map_range, <small>fkyaml::basic_node::</small>const_map_range

```cpp
using map_range = detail::map_range_proxy<basic_node>;
using const_map_range = detail::map_range_proxy<const basic_node>;
```

The helper types for the [`map_items()`](map_items.md) function which allow accessing the `key()` and `value()` functions provided by `iterator` and `const_iterator` in range-based for loops.  
They provides minimal functions and operators which are required for range-based for loops.  

## **Member Types**

| Type             | Definition                                                                                    |
| ---------------- | --------------------------------------------------------------------------------------------- |
| `iterator`       | [LegacyForwardIterator](https://en.cppreference.com/w/cpp/named_req/ForwardIterator)          |
| `const_iterator` | constant [LegacyForwardIterator](https://en.cppreference.com/w/cpp/named_req/ForwardIterator) |

## **Member Functions**

| Name  | Description                                                                    |
| ----- | ------------------------------------------------------------------------------ |
| begin | Returns a (constant) iterator to the first element of mapping entries.         |
| end   | Returns a (constant) iterator to the past-the-last element of mapping entries. |

### **See Also**

* [basic_node](index.md)
* [map_items](map_items.md)
