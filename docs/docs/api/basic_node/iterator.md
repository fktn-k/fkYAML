<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>iterator, <small>fkyaml::basic_node::</small>const_iterator

```cpp
using iterator = detail::iterator<basic_node>;
using iterator = detail::iterator<const basic_node>;
```

The types of non-const/const iterators for [`basic_node`](index.md) containers.  
They satisfies [LegacyBidirectionalIterator](https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator) and are commonly used for sequence and mapping container nodes.  

## **Member Types**

* iterator  
  | Type                | Definition                                                                                    |
  | ------------------- | --------------------------------------------------------------------------------------------- |
  | `value_type`        | `basic_node`                                                                                  |
  | `reference`         | `basic_node&`                                                                                 |
  | `pointer`           | `basic_node*`                                                                                 |
  | `difference_type`   | [`std::ptrdiff_t`](https://en.cppreference.com/w/cpp/types/ptrdiff_t)                         |
  | `iterator_category` | [`std::bidirectional_iterator_tag`](https://en.cppreference.com/w/cpp/iterator/iterator_tags) |
* const_iterator  
  | Type                | Definition                                                                                    |
  | ------------------- | --------------------------------------------------------------------------------------------- |
  | `value_type`        | `basic_node`                                                                                  |
  | `reference`         | `const basic_node&`                                                                           |
  | `pointer`           | `const basic_node*`                                                                           |
  | `difference_type`   | [`std::ptrdiff_t`](https://en.cppreference.com/w/cpp/types/ptrdiff_t)                         |
  | `iterator_category` | [`std::bidirectional_iterator_tag`](https://en.cppreference.com/w/cpp/iterator/iterator_tags) |

## **Member Functions**

| Name  | Description                                                                                                                                        |
| ----- | -------------------------------------------------------------------------------------------------------------------------------------------------- |
| key   | Returns const reference to a key node of the current key-value pair.<br>`fkyaml::exception` is thrown if an iterator points to a sequence element. |
| value | Returns reference to a sequence element if an element points to a sequence element, or a mapping value otherwise.                                  |

## **Non-Member Functions**

| Name               | Description                                                                                                                                                                        |
| ------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| get<std::size_t I> | Returns reference to a key (if I == 0) or a value (if I == 1) mainly for structured binding support.<br>`fkyaml::exception` is thrown if an iterator points to a sequence element. |

## **Examples**

??? Example

    ```cpp
    --8<-- "apis/basic_node/iterator.cpp:9"
    ```

    output:
    ```bash
    --8<-- "apis/basic_node/iterator.output"
    ```

### **See Also**

* [basic_node](index.md)
* [begin, cbegin](begin.md)
* [end, cend](end.md)
* [operator<<](insertion_operator.md)
