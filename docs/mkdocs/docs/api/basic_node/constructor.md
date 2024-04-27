<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>(constructor)

```cpp
basic_node() = default; // (1)

explicit basic_node(const node_t type); // (2)

basic_node(const basic_node& rhs); // (3)

basic_node(basic_node&& rhs) noexcept; // (4)

template <
    typename CompatibleType, typename U = detail::remove_cv_ref_t<CompatibleType>,
    detail::enable_if_t<
        detail::conjunction<
            detail::negation<detail::is_basic_json<U>>,
            detail::disjunction<detail::is_node_compatible_type<basic_node, U>>>::value,
        int> = 0>
basic_node(CompatibleType&& val) noexcept(
    noexcept(ConverterType<U>::to_node(std::declval<basic_node&>(), std::declval<CompatibleType>()))); // (5)

template <
    typename NodeRefStorageType,
    detail::enable_if_t<detail::is_node_ref_storage<NodeRefStorageType>::value, int> = 0>
basic_node(const NodeRefStorageType& node_ref_storage) noexcept; // (6)

basic_node(initializer_list_t init); // (7)
```

Constructs a new basic_node from a variety of data sources.  
Available overloads are described down below.  

## Overload (1)

```cpp
basic_node() = default;
```
Default constructor. Constructs a basic_node with a null value.  
The resulting basic_node has the [`node_t::NULL_OBJECT`](node_t.md) type.  

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_constructor_1.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_constructor_1.output"
    ```

## Overload (2)

```cpp
explicit basic_node(const node_t type);
```
Constructs a basic_node with the given type.  
The resulting basic_node has a default value for the given type.  

### **Parameters**

***`type`*** [in]
:   A YAML node type.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_constructor_2.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_constructor_2.output"
    ```

## Overload (3)

```cpp
basic_node(const basic_node& rhs);
```
Copy constructor. Copies the internal data of `rhs` into the resulting basic_node.  
The resulting basic_node has the same type and value as `rhs`.  

### **Parameters**

***`rhs`*** [in]
:   A basic_node object to be copied with.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_constructor_3.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_constructor_3.output"
    ```

## Overload (4)

```cpp
basic_node(basic_node&& rhs) noexcept;
```
Move constructor. Move the internal data of `rhs` into the resulting basic_node.  
The resulting basic_node has the same type and value as `rhs`.  
The value of the argument `rhs` after calling this move constructor, will be the same as a default-constructed basic_node.  

### **Parameters**

***`rhs`*** [in]
:   A basic_node object to be moved from.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_constructor_4.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_constructor_4.output"
    ```

## Overload (5)

```cpp
template <
    typename CompatibleType, typename U = detail::remove_cv_ref_t<CompatibleType>,
    detail::enable_if_t<
        detail::conjunction<
            detail::negation<detail::is_basic_json<U>>,
            detail::disjunction<detail::is_node_compatible_type<basic_node, U>>>::value,
        int> = 0>
basic_node(CompatibleType&& val) noexcept(
    noexcept(ConverterType<U>::to_node(std::declval<basic_node&>(), std::declval<CompatibleType>()))); // (5)
```
Constructs a basic_node with a value of a compatible type.  
The resulting basic_node has the value of `val` and the type which is associated with `CompatibleType`.  

### **Template Parameters**

***`CompatibleType`***
:   Type of native data which is compatible with node values.

***`U`***
:   Type of compatible native data without cv-qualifiers and reference.

### **Parameters**

***`val`*** [in]
:   The value of a compatible type.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_constructor_5.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_constructor_5.output"
    ```

## Overload (6)

```cpp
template <
    typename NodeRefStorageType,
    detail::enable_if_t<detail::is_node_ref_storage<NodeRefStorageType>::value, int> = 0>
basic_node(const NodeRefStorageType& node_ref_storage) noexcept;
```
Constructs a basic_node with a node_ref_storage.  
The resulting basic_node has the value of the referenced basic_node by `node_ref_storage`.  

!!! Warning

    This constructor is mainly for the one with std::initializer_list, and the argument type is too detailed to be used from outside the fkYAML library.  
    So, this overload might become unavailable in the future major version.  
    Please refrain from intentionally using this overload, and use the overload with std::initializer_list instead.

### **Template Parameters**

***`NodeRefStorageType`***
:   Type of basic_node with reference.

### **Parameters**

***`node_ref_storage`*** [in]
:   A node_ref_storage template class object.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_constructor_6.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_constructor_6.output"
    ```

## Overload (7)

```cpp
basic_node(initializer_list_t init);
```
Constructs a basic_node with a initializer_list_t object.  
The resulting basic_node has the value of a container (sequence or mapping) which has the contents of `init`.  
Basically, the basic_node objects in `init` are considered as a sequence node.
If `init` contains a sequence of basic_node objects in which the number of basic_node objects is 2 and the first has the type of `node_t::STRING`, however, such a sequence is reinterpreted as a mapping node.  

### **Parameters**

***`init`*** [in]
:   A initializer list of basic_node objects.

???+ Example

    ```cpp
    --8<-- "examples/ex_basic_node_constructor_7.cpp:9"
    ```

    output:
    ```bash
    --8<-- "examples/ex_basic_node_constructor_7.output"
    ```
---

## **See Also**

* [basic_node](index.md)
* [operator<<](insertion_operator.md)
