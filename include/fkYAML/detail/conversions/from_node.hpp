//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_CONVERSIONS_FROM_NODE_HPP
#define FK_YAML_DETAIL_CONVERSIONS_FROM_NODE_HPP

#include <array>
#include <cmath>
#include <forward_list>
#include <limits>
#include <utility>
#include <valarray>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>
#include <fkYAML/detail/types/node_t.hpp>
#include <fkYAML/exception.hpp>

#ifdef FK_YAML_HAS_CXX_17
#include <optional>
#endif

FK_YAML_DETAIL_NAMESPACE_BEGIN

///////////////////
//   from_node   //
///////////////////

// utility type traits and functors

/// @brief Utility traits type alias to detect constructible associative container types from a mapping node, e.g.,
/// std::map or std::unordered_map.
/// @tparam T A target type for detection.
template <typename T>
using is_constructible_mapping_type =
    conjunction<detect::has_key_type<T>, detect::has_mapped_type<T>, detect::has_value_type<T>>;

/// @brief Utility traits type alias to detect constructible container types from a sequence node, e.g., std::vector or
/// std::list.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A target type for detection.
template <typename BasicNodeType, typename T>
using is_constructible_sequence_type = conjunction<
    negation<is_basic_node<T>>, detect::has_iterator<T>, detect::is_iterator_traits<typename T::iterator>,
    detect::has_begin_end<T>, negation<std::is_same<T, typename BasicNodeType::mapping_type>>,
    negation<is_constructible_mapping_type<T>>>;

/// @brief Utility traits type alias to detect a sequence container adapter type, e.g., std::stack or std::queue.
/// @tparam T A target type for detection.
template <typename T>
using is_sequence_container_adapter = conjunction<
    negation<is_basic_node<T>>, detect::has_container_type<T>, detect::has_value_type<T>,
    negation<detect::has_key_type<T>>>;

/// @brief Helper struct for reserve() member function call switch for types which do not have reserve function.
/// @tparam ContainerType A container type.
template <typename ContainerType, typename = void>
struct call_reserve_if_available {
    /// @brief Do nothing since ContainerType does not have reserve function.
    static void call(ContainerType& /*unused*/, typename ContainerType::size_type /*unused*/) {
    }
};

/// @brief Helper struct for reserve() member function call switch for types which have reserve function.
/// @tparam ContainerType A container type.
template <typename ContainerType>
struct call_reserve_if_available<ContainerType, enable_if_t<detect::has_reserve<ContainerType>::value>> {
    /// @brief Call reserve function on the ContainerType object with a given size.
    /// @param c A container object.
    /// @param n A size to reserve.
    static void call(ContainerType& c, typename ContainerType::size_type n) {
        c.reserve(n);
    }
};

// from_node() implementations

/// @brief from_node function for C-style 1D arrays whose element type must be a basic_node template instance type or a
/// compatible type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T Element type of C-style 1D array.
/// @tparam N Size of the array.
/// @param n A basic_node object.
/// @param array An array object.
template <typename BasicNodeType, typename T, std::size_t N>
inline auto from_node(const BasicNodeType& n, T (&array)[N])
    -> decltype(n.get_value_inplace(std::declval<T&>()), void()) {
    if FK_YAML_UNLIKELY (!n.is_sequence()) {
        throw type_error("The target node value type is not sequence type.", n.get_type());
    }

    // call get_value_inplace(), not get_value(), since the storage to fill the result into is already created.
    for (std::size_t i = 0; i < N; i++) {
        n.at(i).get_value_inplace(array[i]);
    }
}

/// @brief from_node function for C-style 2D arrays whose element type must be a basic_node template instance type or a
/// compatible type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T Element type of C-style 2D array.
/// @tparam N0 Size of the outer dimension.
/// @tparam N1 Size of the inner dimension.
/// @param n A basic_node object.
/// @param array An array object.
template <typename BasicNodeType, typename T, std::size_t N0, std::size_t N1>
inline auto from_node(const BasicNodeType& n, T (&array)[N0][N1])
    -> decltype(n.get_value_inplace(std::declval<T&>()), void()) {
    if FK_YAML_UNLIKELY (!n.is_sequence()) {
        throw type_error("The target node value type is not sequence type.", n.get_type());
    }

    // call get_value_inplace(), not get_value(), since the storage to fill the result into is already created.
    for (std::size_t i0 = 0; i0 < N0; i0++) {
        for (std::size_t i1 = 0; i1 < N1; i1++) {
            n.at(i0).at(i1).get_value_inplace(array[i0][i1]);
        }
    }
}

/// @brief from_node function for C-style 2D arrays whose element type must be a basic_node template instance type or a
/// compatible type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T Element type of C-style 2D array.
/// @tparam N0 Size of the outermost dimension.
/// @tparam N1 Size of the middle dimension.
/// @tparam N2 Size of the innermost dimension.
/// @param n A basic_node object.
/// @param array An array object.
template <typename BasicNodeType, typename T, std::size_t N0, std::size_t N1, std::size_t N2>
inline auto from_node(const BasicNodeType& n, T (&array)[N0][N1][N2])
    -> decltype(n.get_value_inplace(std::declval<T&>()), void()) {
    if FK_YAML_UNLIKELY (!n.is_sequence()) {
        throw type_error("The target node value type is not sequence type.", n.get_type());
    }

    // call get_value_inplace(), not get_value(), since the storage to fill the result into is already created.
    for (std::size_t i0 = 0; i0 < N0; i0++) {
        for (std::size_t i1 = 0; i1 < N1; i1++) {
            for (std::size_t i2 = 0; i2 < N2; i2++) {
                n.at(i0).at(i1).at(i2).get_value_inplace(array[i0][i1][i2]);
            }
        }
    }
}

/// @brief from_node function for std::array objects whose element type must be a basic_node template instance type or a
/// compatible type. This function is necessary since insert function is not implemented for std::array.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T Element type of std::array.
/// @tparam N Size of std::array.
/// @param n A basic_node object.
/// @param arr A std::array object.
template <typename BasicNodeType, typename T, std::size_t N>
inline auto from_node(const BasicNodeType& n, std::array<T, N>& arr)
    -> decltype(n.get_value_inplace(std::declval<T&>()), void()) {
    if FK_YAML_UNLIKELY (!n.is_sequence()) {
        throw type_error("The target node value type is not sequence type.", n.get_type());
    }

    for (std::size_t i = 0; i < N; i++) {
        // call get_value_inplace(), not get_value(), since the storage to fill the result into is already created.
        n.at(i).get_value_inplace(arr.at(i));
    }
}

/// @brief from_node function for std::valarray objects whose element type must be a basic_node template instance type
/// or a compatible type. This function is necessary since insert function is not implemented for std::valarray.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T Element type of std::valarray.
/// @param n A basic_node object.
/// @param va A std::valarray object.
template <typename BasicNodeType, typename T>
inline auto from_node(const BasicNodeType& n, std::valarray<T>& va)
    -> decltype(n.get_value_inplace(std::declval<T&>()), void()) {
    if FK_YAML_UNLIKELY (!n.is_sequence()) {
        throw type_error("The target node value type is not sequence type.", n.get_type());
    }

    std::size_t count = n.size();
    va.resize(count);
    for (std::size_t i = 0; i < count; i++) {
        // call get_value_inplace(), not get_value(), since the storage to fill the result into is already created.
        n.at(i).get_value_inplace(va[i]);
    }
}

/// @brief from_node function for std::forward_list objects whose element type must be a basic_node template instance
/// type or a compatible type. This function is necessary since insert function is not implemented for
/// std::forward_list.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T Element type of std::forward_list.
/// @tparam Alloc Allocator type of std::forward_list.
/// @param n A basic_node object.
/// @param fl A std::forward_list object.
template <typename BasicNodeType, typename T, typename Alloc>
inline auto from_node(const BasicNodeType& n, std::forward_list<T, Alloc>& fl)
    -> decltype(n.template get_value<T>(), void()) {
    if FK_YAML_UNLIKELY (!n.is_sequence()) {
        throw type_error("The target node value is not sequence type.", n.get_type());
    }

    fl.clear();

    // std::forward_list does not have insert function.
    auto insert_pos_itr = fl.before_begin();
    for (const auto& elem : n) {
        insert_pos_itr = fl.emplace_after(insert_pos_itr, elem.template get_value<T>());
    }
}

/// @brief from_node function for container objects of only keys or values, e.g., std::vector or std::set, whose element
/// type must be a basic_node template instance type or a compatible type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam CompatSeqType A container type.
/// @param n A basic_node object.
/// @param s A container object.
template <
    typename BasicNodeType, typename CompatSeqType,
    enable_if_t<
        conjunction<
            is_basic_node<BasicNodeType>, is_constructible_sequence_type<BasicNodeType, CompatSeqType>,
            negation<std::is_constructible<typename BasicNodeType::string_type, CompatSeqType>>>::value,
        int> = 0>
inline auto from_node(const BasicNodeType& n, CompatSeqType& s)
    -> decltype(n.template get_value<typename CompatSeqType::value_type>(), void()) {
    if FK_YAML_UNLIKELY (!n.is_sequence()) {
        throw type_error("The target node value is not sequence type.", n.get_type());
    }

    s.clear();

    // call reserve function first if it's available (like std::vector).
    call_reserve_if_available<CompatSeqType>::call(s, n.size());

    // transform a sequence node into a destination type object by calling insert function.
    using std::end;
    std::transform(n.begin(), n.end(), std::inserter(s, end(s)), [](const BasicNodeType& elem) {
        return elem.template get_value<typename CompatSeqType::value_type>();
    });
}

/// @brief from_node function for sequence container adapter objects, e.g., std::stack or std::queue, whose element type
/// must be either a basic_node template instance type or a compatible type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam SeqContainerAdapter A sequence container adapter type.
/// @param n A node object.
/// @param ca A sequence container adapter object.
template <
    typename BasicNodeType, typename SeqContainerAdapter,
    enable_if_t<
        conjunction<is_basic_node<BasicNodeType>, is_sequence_container_adapter<SeqContainerAdapter>>::value, int> = 0>
inline auto from_node(const BasicNodeType& n, SeqContainerAdapter& ca)
    -> decltype(n.template get_value<typename SeqContainerAdapter::value_type>(), ca.push(std::declval<typename SeqContainerAdapter::value_type>()), void()) {
    if FK_YAML_UNLIKELY (!n.is_sequence()) {
        throw type_error("The target node value is not sequence type.", n.get_type());
    }

    // clear existing elements manually since clear function is not implemeneted for container adapter classes.
    while (!ca.empty()) {
        ca.pop();
    }

    for (const auto& elem : n) {
        // container adapter classes commonly have push function.
        // emplace function cannot be used in case SeqContainerAdapter::container_type is std::vector<bool> in C++11.
        ca.push(elem.template get_value<typename SeqContainerAdapter::value_type>());
    }
}

/// @brief from_node function for mappings whose key and value are of both compatible types.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam CompatibleKeyType Mapping key type compatible with BasicNodeType.
/// @tparam CompatibleValueType Mapping value type compatible with BasicNodeType.
/// @tparam Compare Comparator type for mapping keys.
/// @tparam Allocator Allocator type for destination mapping object.
/// @param n A node object.
/// @param m Mapping container object to store converted key/value objects.
template <typename BasicNodeType, typename CompatMapType, enable_if_t<is_constructible_mapping_type<CompatMapType>::value, int> = 0>
inline auto from_node(const BasicNodeType& n, CompatMapType& m)
    -> decltype(
        std::declval<const BasicNodeType&>().template get_value<typename CompatMapType::key_type>(),
        std::declval<const BasicNodeType&>().template get_value<typename CompatMapType::mapped_type>(),
        m.emplace(std::declval<typename CompatMapType::key_type>(), std::declval<typename CompatMapType::mapped_type>()),
        void()) {
    if FK_YAML_UNLIKELY (!n.is_mapping()) {
        throw type_error("The target node value type is not mapping type.", n.get_type());
    }

    m.clear();
    call_reserve_if_available<CompatMapType>::call(m, n.size());

    for (const auto& pair : n.as_map()) {
        m.emplace(
            pair.first.template get_value<typename CompatMapType::key_type>(),
            pair.second.template get_value<typename CompatMapType::mapped_type>());
    }
}

/// @brief from_node function for nullptr.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A node object.
/// @param null Storage for a null value.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, std::nullptr_t& null) {
    // to ensure the target node value type is null.
    if FK_YAML_UNLIKELY (!n.is_null()) {
        throw type_error("The target node value type is not null type.", n.get_type());
    }
    null = nullptr;
}

/// @brief from_node function for booleans.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A node object.
/// @param b Storage for a boolean value.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, bool& b) {
    switch (n.get_type()) {
    case node_type::NULL_OBJECT:
        // nullptr is converted to false just as C++ implicitly does.
        b = false;
        break;
    case node_type::BOOLEAN:
        b = static_cast<bool>(n.as_bool());
        break;
    case node_type::INTEGER:
        // true: non-zero, false: zero
        b = (n.as_int() != 0);
        break;
    case node_type::FLOAT:
        // true: non-zero, false: zero
        using float_type = typename BasicNodeType::float_number_type;
        b = (n.as_float() != static_cast<float_type>(0.));
        break;
    case node_type::SEQUENCE:
    case node_type::MAPPING:
    case node_type::STRING:
    default:
        throw type_error("The target node value type is not compatible with boolean type.", n.get_type());
    }
}

/// @brief Helper struct for node-to-int conversion.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam IntType Target integer value type (same as BasicNodeType::integer_type)
template <
    typename BasicNodeType, typename IntType, bool = std::is_same<typename BasicNodeType::integer_type, IntType>::value>
struct from_node_int_helper {
    /// @brief Convert node's integer value to the target integer type.
    /// @param n A node object.
    /// @return An integer value converted from the node's integer value.
    static IntType convert(const BasicNodeType& n) {
        return n.as_int();
    }
};

/// @brief Helper struct for node-to-int conversion if IntType is not the node's integer value type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam IntType Target integer value type (different from BasicNodeType::integer_type)
template <typename BasicNodeType, typename IntType>
struct from_node_int_helper<BasicNodeType, IntType, false> {
    /// @brief Convert node's integer value to non-uint64_t integer types.
    /// @param n A node object.
    /// @return An integer value converted from the node's integer value.
    static IntType convert(const BasicNodeType& n) {
        using node_int_type = typename BasicNodeType::integer_type;
        const node_int_type tmp_int = n.as_int();

        // under/overflow check.
        if (std::is_same<IntType, uint64_t>::value) {
            if FK_YAML_UNLIKELY (tmp_int < 0) {
                throw exception("Integer value underflow detected.");
            }
        }
        else {
            if FK_YAML_UNLIKELY (tmp_int < static_cast<node_int_type>(std::numeric_limits<IntType>::min())) {
                throw exception("Integer value underflow detected.");
            }
            if FK_YAML_UNLIKELY (static_cast<node_int_type>(std::numeric_limits<IntType>::max()) < tmp_int) {
                throw exception("Integer value overflow detected.");
            }
        }

        return static_cast<IntType>(tmp_int);
    }
};

/// @brief from_node function for integers.
/// @note If node's value is null, boolean, or float, such a value is converted into an integer internally.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam IntegerType An integer value type.
/// @param n A node object.
/// @param i Storage for an integer value.
template <
    typename BasicNodeType, typename IntegerType,
    enable_if_t<conjunction<is_basic_node<BasicNodeType>, is_non_bool_integral<IntegerType>>::value, int> = 0>
inline void from_node(const BasicNodeType& n, IntegerType& i) {
    switch (n.get_type()) {
    case node_type::NULL_OBJECT:
        // nullptr is interpreted as 0
        i = static_cast<IntegerType>(0);
        break;
    case node_type::BOOLEAN:
        i = static_cast<bool>(n.as_bool()) ? static_cast<IntegerType>(1) : static_cast<IntegerType>(0);
        break;
    case node_type::INTEGER:
        i = from_node_int_helper<BasicNodeType, IntegerType>::convert(n);
        break;
    case node_type::FLOAT: {
        // int64_t should be safe to express the integer part of possible floating point types.
        const auto tmp_int = static_cast<int64_t>(n.as_float());

        // under/overflow check.
        if (std::is_same<IntegerType, uint64_t>::value) {
            if FK_YAML_UNLIKELY (tmp_int < 0) {
                throw exception("Integer value underflow detected.");
            }
        }
        else {
            if FK_YAML_UNLIKELY (tmp_int < static_cast<int64_t>(std::numeric_limits<IntegerType>::min())) {
                throw exception("Integer value underflow detected.");
            }
            if FK_YAML_UNLIKELY (static_cast<int64_t>(std::numeric_limits<IntegerType>::max()) < tmp_int) {
                throw exception("Integer value overflow detected.");
            }
        }

        i = static_cast<IntegerType>(tmp_int);
        break;
    }
    case node_type::SEQUENCE:
    case node_type::MAPPING:
    case node_type::STRING:
    default:
        throw type_error("The target node value type is not compatible with integer type.", n.get_type());
    }
}

/// @brief Helper struct for node-to-float conversion if FloatType is the node's floating point value type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam FloatType Target floating point value type (same as the BasicNodeType::float_number_type)
template <
    typename BasicNodeType, typename FloatType,
    bool = std::is_same<typename BasicNodeType::float_number_type, FloatType>::value>
struct from_node_float_helper {
    /// @brief Convert node's floating point value to the target floating point type.
    /// @param n A node object.
    /// @return A floating point value converted from the node's floating point value.
    static FloatType convert(const BasicNodeType& n) {
        return n.as_float();
    }
};

/// @brief Helper struct for node-to-float conversion if IntType is not the node's floating point value type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam FloatType Target floating point value type (different from BasicNodeType::float_number_type)
template <typename BasicNodeType, typename FloatType>
struct from_node_float_helper<BasicNodeType, FloatType, false> {
    /// @brief Convert node's floating point value to the target floating point type.
    /// @param n A node object.
    /// @return A floating point value converted from the node's floating point value.
    static FloatType convert(const BasicNodeType& n) {
        using node_float_type = typename BasicNodeType::float_number_type;
        auto tmp_float = n.as_float();

        // check if the value is an infinite number (either positive or negative)
        if (std::isinf(tmp_float)) {
            if (tmp_float == std::numeric_limits<node_float_type>::infinity()) {
                return std::numeric_limits<FloatType>::infinity();
            }

            return static_cast<FloatType>(-1.) * std::numeric_limits<FloatType>::infinity();
        }

        // check if the value is not a number
        if (std::isnan(tmp_float)) {
            return std::numeric_limits<FloatType>::quiet_NaN();
        }

        // check if the value is expressible as FloatType.
        if FK_YAML_UNLIKELY (tmp_float < std::numeric_limits<FloatType>::lowest()) {
            throw exception("Floating point value underflow detected.");
        }
        if FK_YAML_UNLIKELY (std::numeric_limits<FloatType>::max() < tmp_float) {
            throw exception("Floating point value overflow detected.");
        }

        return static_cast<FloatType>(tmp_float);
    }
};

/// @brief from_node function for floating point values.
/// @note If node's value is null, boolean, or integer, such a value is converted into a floating point internally.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam FloatType A floating point value type.
/// @param n A node object.
/// @param f Storage for a float point value.
template <
    typename BasicNodeType, typename FloatType,
    enable_if_t<conjunction<is_basic_node<BasicNodeType>, std::is_floating_point<FloatType>>::value, int> = 0>
inline void from_node(const BasicNodeType& n, FloatType& f) {
    switch (n.get_type()) {
    case node_type::NULL_OBJECT:
        // nullptr is interpreted as 0.0
        f = static_cast<FloatType>(0.);
        break;
    case node_type::BOOLEAN:
        f = static_cast<bool>(n.as_bool()) ? static_cast<FloatType>(1.) : static_cast<FloatType>(0.);
        break;
    case node_type::INTEGER:
        f = static_cast<FloatType>(n.as_int());
        break;
    case node_type::FLOAT:
        f = from_node_float_helper<BasicNodeType, FloatType>::convert(n);
        break;
    case node_type::SEQUENCE:
    case node_type::MAPPING:
    case node_type::STRING:
    default:
        throw type_error("The target node value type is not compatible with float number type.", n.get_type());
    }
}

/// @brief from_node function for BasicNodeType::string_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param s A string node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, typename BasicNodeType::string_type& s) {
    if FK_YAML_UNLIKELY (!n.is_string()) {
        throw type_error("The target node value type is not string type.", n.get_type());
    }
    s = n.as_str();
}

/// @brief from_node function for compatible string type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam CompatibleStringType A compatible string type.
/// @param n A basic_node object.
/// @param s A compatible string object.
template <
    typename BasicNodeType, typename CompatibleStringType,
    enable_if_t<
        conjunction<
            is_basic_node<BasicNodeType>,
            negation<std::is_same<CompatibleStringType, typename BasicNodeType::string_type>>,
            disjunction<
                std::is_constructible<CompatibleStringType, const typename BasicNodeType::string_type&>,
                std::is_assignable<CompatibleStringType, const typename BasicNodeType::string_type&>>>::value,
        int> = 0>
inline void from_node(const BasicNodeType& n, CompatibleStringType& s) {
    if FK_YAML_UNLIKELY (!n.is_string()) {
        throw type_error("The target node value type is not string type.", n.get_type());
    }
    s = n.as_str();
}

/// @brief from_node function for std::pair objects whose element types must be either a basic_node template instance
/// type or a compatible type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T The first type of the std::pair.
/// @tparam U The second type of the std::pair.
/// @param n A basic_node object.
/// @param p A std::pair object.
template <typename BasicNodeType, typename T, typename U, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline auto from_node(const BasicNodeType& n, std::pair<T, U>& p)
    -> decltype(std::declval<const BasicNodeType&>().template get_value<T>(), std::declval<const BasicNodeType&>().template get_value<U>(), void()) {
    if FK_YAML_UNLIKELY (!n.is_sequence()) {
        throw type_error("The target node value type is not sequence type.", n.get_type());
    }

    // call get_value_inplace(), not get_value(), since the storage to fill the result into is already created.
    n.at(0).get_value_inplace(p.first);
    n.at(1).get_value_inplace(p.second);
}

/// @brief concrete implementation of from_node function for std::tuple objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam ...Types The value types of std::tuple.
/// @tparam ...Idx Index sequence values for std::tuples value types.
/// @param n A basic_node object
/// @param _ Index sequence values (unused).
/// @return A std::tuple object converted from the sequence node values.
template <typename BasicNodeType, typename... Types, std::size_t... Idx>
inline std::tuple<Types...> from_node_tuple_impl(const BasicNodeType& n, index_sequence<Idx...> /*unused*/) {
    return std::make_tuple(n.at(Idx).template get_value<Types>()...);
}

/// @brief from_node function for std::tuple objects whose value types must all be either a basic_node template instance
/// type or a compatible type
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam ...Types Value types of std::tuple.
/// @param n A basic_node object.
/// @param t A std::tuple object.
template <typename BasicNodeType, typename... Types, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, std::tuple<Types...>& t) {
    if FK_YAML_UNLIKELY (!n.is_sequence()) {
        throw type_error("The target node value type is not sequence type.", n.get_type());
    }

    // Types... must be explicitly specified; the retun type would otherwise be std::tuple with no value types.
    t = from_node_tuple_impl<BasicNodeType, Types...>(n, index_sequence_for<Types...> {});
}

#ifdef FK_YAML_HAS_CXX_17

/// @brief from_node function for std::optional objects whose value type must be either a basic_node template instance
/// type or a compatible type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A value type of the std::optional.
/// @param n A basic_node object.
/// @param o A std::optional object.
template <typename BasicNodeType, typename T, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline auto from_node(const BasicNodeType& n, std::optional<T>& o) -> decltype(n.template get_value<T>(), void()) {
    try {
        o.emplace(n.template get_value<T>());
    }
    catch (const std::exception& /*unused*/) {
        // Any exception derived from std::exception is interpreted as a conversion failure in some way
        // since user-defined from_node function may throw a different object from a fkyaml::type_error.
        // and std::exception is usually the base class of user-defined exception types.
        o = std::nullopt;
    }
}

#endif // defined(FK_YAML_HAS_CXX_17)

/// @brief A function object to call from_node functions.
/// @note User-defined specialization is available by providing implementation **OUTSIDE** fkyaml namespace.
struct from_node_fn {
    /// @brief Call from_node function suitable for the given T type.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @tparam T A target value type assigned from the basic_node object.
    /// @param n A basic_node object.
    /// @param val A target object assigned from the basic_node object.
    /// @return decltype(from_node(n, std::forward<T>(val))) void by default. User can set it to some other type.
    template <typename BasicNodeType, typename T>
    auto operator()(const BasicNodeType& n, T&& val) const
        noexcept(noexcept(from_node(n, std::forward<T>(val)))) -> decltype(from_node(n, std::forward<T>(val))) {
        return from_node(n, std::forward<T>(val));
    }
};

FK_YAML_DETAIL_NAMESPACE_END

FK_YAML_NAMESPACE_BEGIN

#ifndef FK_YAML_HAS_CXX_17
// anonymous namespace to hold `from_node` functor.
// see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html for why it's needed.
namespace // NOLINT(cert-dcl59-cpp,fuchsia-header-anon-namespaces,google-build-namespaces)
{
#endif

/// @brief A global object to represent ADL friendly from_node functor.
// NOLINTNEXTLINE(misc-definitions-in-headers)
FK_YAML_INLINE_VAR constexpr const auto& from_node = detail::static_const<detail::from_node_fn>::value;

#ifndef FK_YAML_HAS_CXX_17
} // namespace
#endif

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_CONVERSIONS_FROM_NODE_HPP */
