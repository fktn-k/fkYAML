<small>Defined in header [`<fkYAML/ordered_map.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/ordered_map.hpp)</small>

# <small>fkyaml::</small>ordered_map

```cpp
template<
    typename Key, typename Value, typename IgnoredCompare = std::less<Key>,
    typename Allocator = std::allocator<std::pair<const Key, Value>>>
class ordered_map : public std::vector<std::pair<const Key, Value>>;
```

A minimal map-like container which preserves insertion order.  
This documentation only describes APIs which are not of the parent class [`std::vector<std::pair<const Key, Value>>`](https://en.cppreference.com/w/cpp/container/vector).  

!!! Question annotate "How is this class useful?"

    This class could be useful in case that **the order of insertion in YAML mapping nodes needs to be preserved**.  
    This is because the YAML specification recommends that [a sequence of mappings should be used in such cases](https://yaml.org/spec/1.2.2/#3221-mapping-key-order), since YAML mapping nodes are defined as *unordered* sets of key-value pairs.  
    However, the above recommendation with a sequence of key-value pair does not work so efficiently on the application layer because it cannot be a direct representation of the deserialization result and could also break the original order when the deserialized node is serialized again.  
    The ordered_map class is thus provided to help resolve those issues.  

## **Template Parameters**

| Template parameter | Description                          | Derived type |
| ------------------ | ------------------------------------ | ------------ |
| `Key`              | type for keys                        | key_type     |
| `Value`            | type for values                      | mapped_type  |
| `IgnoredCompare`   | placeholder type for key comparisons | -            |
| `Allocator`        | type for allocators                  | -            |


## **Member Types**

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

## **Member Functions**

### Construction/Destruction
| Name                            | Description                |
| ------------------------------- | -------------------------- |
| [(constructor)](constructor.md) | constructs an ordered_map. |
| [(destructor)](destructor.md)   | destroys an ordered_map.   |

### Element Access

| Name                        | Description                                       |
| --------------------------- | ------------------------------------------------- |
| [at](at.md)                 | forces to accesses an element with the given key. |
| [operator[]](operator[].md) | accesses an element with the given key.           |

### Modifiers

| Name                  | Description                                                  |
| --------------------- | ------------------------------------------------------------ |
| [emplace](emplace.md) | emplaces a new key-value pair if the new key does not exist. |

### Lookup

| Name            | Description                                |
| --------------- | ------------------------------------------ |
| [find](find.md) | finds a value associated to the given key. |
