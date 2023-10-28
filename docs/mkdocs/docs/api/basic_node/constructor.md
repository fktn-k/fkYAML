<small>Defined in header [`<fkYAML/node.hpp>`](https://github.com/fktn-k/fkYAML/blob/develop/include/fkYAML/node.hpp)</small>

# <small>fkyaml::basic_node::</small>(constructor)

Constructs new basic_node from a variety of data sources.  
Available overloads are described down below.  

## Overloads

```cpp
basic_node() = default;
```
Default constructor. Constructs a basic_node with a null value.  
The resulting basic_node has the [`node_t::NULL_OBJECT`](node_t.md) type.  

??? example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        fkyaml::node n;
        std::cout << fkyaml::node::serialize(n) << std::endl;
        return 0;
    }
    ```
    output:

    ```yaml
    null
    ```
---

```cpp
explicit basic_node(const node_t type);
```
Constructs a basic_node with the given type.  
The resulting basic_node has a default value for the given type.  

### **Parameters**

***`type`*** [in]
:   A YAML node value.

??? example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        fkyaml::node n(fkyaml::node::node_t::INTEGER);
        std::cout << fkyaml::node::serialize(n) << std::endl;
        return 0;
    }
    ```
    output:

    ```yaml
    0
    ```
---

```cpp
basic_node(const basic_node& rhs);
```
Copy constructor. Copies the internal data of `rhs` into the resulting basic_node.  
The resulting basic_node has the same type and value as `rhs`.  

### **Parameters**

***`rhs`*** [in]
:   A basic node to be copied with.

??? example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        fkyaml::node n(fkyaml::node::node_t::BOOLEAN);
        fkyaml::node n2(n);
        std::cout << fkyaml::node::serialize(n) << std::endl;
        return 0;
    }
    ```
    output:

    ```yaml
    false
    ```
---

```cpp
basic_node(basic_node&& rhs) noexcept;
```
Move constructor. Move the internal data of `rhs` into the resulting basic_node.  
The resulting basic_node has the same type and value as `rhs`.  
The value of the argument `rhs` after calling this move constructor, will be the same as a default-constructed basic_node.  

### **Parameters**

***`rhs`*** [in]
:   A basic node to be moved from.

??? example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        fkyaml::node n(fkyaml::node::node_t::BOOLEAN);
        fkyaml::node n2(n);
        std::cout << fkyaml::node::serialize(n) << std::endl;
        return 0;
    }
    ```
    output:

    ```yaml
    false
    ```
---

```cpp
template <
    typename CompatibleType, typename U = detail::remove_cv_ref_t<CompatibleType>,
    detail::enable_if_t<
        detail::conjunction<
            detail::negation<detail::is_basic_json<U>>,
            detail::disjunction<detail::is_node_compatible_type<basic_node, U>>>::value,
        int> = 0>
basic_node(CompatibleType&& val);
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

??? example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        double pi = 3.141592;
        fkyaml::node n(pi);
        std::cout << fkyaml::node::serialize(n) << std::endl;
        return 0;
    }
    ```
    output:

    ```yaml
    3.141592
    ```
---

```cpp
template <
    typename NodeRefStorageType,
    detail::enable_if_t<detail::is_node_ref_storage<NodeRefStorageType>::value, int> = 0>
basic_node(const NodeRefStorageType& node_ref_storage);
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

??? example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        fkyaml::node n({true, false});
        std::cout << fkyaml::node::serialize(n) << std::endl;
        return 0;
    }
    ```
    output:

    ```yaml
    - true
    - false
    ```
---

```cpp
basic_node(initializer_list_t init);
```
Constructs a basic_node with a initializer_list_t object.  
The resulting basic_node has the value of a container (sequence or mapping) which has the contents of `init`.  
Basically, the basic_node objects in `init` are considered as a sequence node.
If `init` contains a sequence of basic_node objects in which the number of basic_node objects is 2 and the first has the type of `node_t::STRING`, however, such a sequence is reinterpreted as a mapping node.  

!!! Note

    To avoid ambiguity between a sequence of `char` and a `std::string`, c-style char arrays are intentionally unsupported in this constructor.  
    To contain a string in a `initializer_list_t` object, you must explicitly pass a `std::string` object as follows:  
    ```cpp
    // this is not supported.
    fkyaml::node n = {"foo", "bar"};

    // you must do this instead.
    fkyaml::node n2 = {std::string("foo"), std::string("bar")};
    ```

### **Parameters**

***`init`*** [in]
:   A initializer list of basic_node objects.

??? example

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        fkyaml::node n = {true, false};
        std::cout << fkyaml::node::serialize(n) << std::endl;

        fkyaml::node n2 = {std::string("foo"), 1024};
        std::cout << fkyaml::node::serialize(n2) << std::endl;
        return 0;
    }
    ```
    output:

    ```yaml
    - true
    - false
    ```
    ```yaml
    foo: 1024
    ```
---

## **See Also**

* [basic_node](index.md)
