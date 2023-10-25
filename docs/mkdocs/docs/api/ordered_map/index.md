<small>Defined in header [`<fkYAML/ordered_map.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/ordered_map.hpp)</small>

# <small>fkyaml::</small>ordered_map

```cpp
template<
    typename Key, typename Value, typename IgnoredCompare = std::less<Key>,
    typename Allocator = std::allocator<std::pair<const Key, Value>>>
class ordered_map;
```

A minimal map-like container which preserves insertion order.

## Template Parameters

| Template parameter | Description                                             | Derived type |
|--------------------|---------------------------------------------------------|--------------|
| `Key`              | type for sequence node value containers                 | key_type     |
| `Value`            | type for mapping node value containers                  | mapped_type  |
| `IgnoredCompare`   | type for boolean node values                            | -            |
| `Allocator`        | type for integer node values                            | -            |


## Member Types

| Name           | Description                                     |
|----------------|-------------------------------------------------|
| key_type       | The type for keys.                              |
| mapped_type    | The type for values.                            |
| Container      | The type for internal key-value containers.     |
| value_type     | The type for key-value pairs.                   |
| iterator       | The type for non-constant iterators.            |
| const_iterator | The type for constant iterators.                |
| size_type      | The type for size parameters used in the class. |
| key_compare    | The type for comparison between keys.           |

## Member Functions