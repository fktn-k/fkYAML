<small>Defined in header [`<fkYAML/node_value_converter.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node_value_converter.hpp)</small>

# <small>fkyaml::</small>node_value_converter

```cpp
template <typename ValueType, typename>
class node_value_converter;
```

An ADL friendly converter between [basic_node](../basic_node/index.md) objects and native data objects.

## **Template Parameters**

| Template parameter | Description                          |
| ------------------ | ------------------------------------ |
| `ValueType`        | The default target native data type. |

## **Member Functions**

### Conversions

| Name                      | Description                               |
| ------------------------- | ----------------------------------------- |
| [from_node](from_node.md) | converts a native data into a basic_node. |
| [to_node](to_node.md)     | converts a basic_node into a native data. |
