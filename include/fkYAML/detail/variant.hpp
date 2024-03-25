///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_VARIANT_HPP_
#define FK_YAML_DETAIL_VARIANT_HPP_

#include <cstring>
#include <type_traits>
#include <utility>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/exception.hpp>

/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

//////////////////////////////////
//   type traits for variants   //
//////////////////////////////////

template <typename... Types>
struct has_duplicate_types;

template <typename First, typename... Rest>
struct has_duplicate_types<First, Rest...>
    : std::conditional<
          disjunction<std::is_same<First, Rest>...>::value, std::true_type, has_duplicate_types<Rest...>>::type
{
};

template <>
struct has_duplicate_types<> : std::false_type
{
};

// forward declaration of the get_type_id_helper_value() function.
template <typename T, typename... Types>
inline constexpr std::size_t get_type_id_helper_value();

/// @brief A helper trait to detect the first appearance of T in Types.
/// @note `get_type_id_helper<...>::value` is `sizeof...(Types)` if T is not in Types.
template <typename T, typename... Types>
struct get_type_id_helper : std::integral_constant<std::size_t, 0>
{
};

template <typename T, typename First, typename... Rest>
struct get_type_id_helper<T, First, Rest...>
    : std::integral_constant<
          std::size_t, std::is_same<T, First>::value ? 0 : get_type_id_helper_value<T, Rest...>() + 1>
{
};

/// @brief Returns `get_type_id_helper<...>::value` at compile time.
template <typename T, typename... Types>
inline constexpr std::size_t get_type_id_helper_value()
{
    // This works like template variables (introduced since C++14)
    return get_type_id_helper<T, Types...>::value;
}

//////////////////////////////////
//   utility alternative tags   //
//////////////////////////////////

/// @brief A tag which specifies that the wrapped pointer type object needs be heap allocated.
/// @note Without this tag, given pointer object just points to some already allocated memory address.
template <typename T>
struct heap_allocate_tag
{
    using type = T*;
};

/////////////////////////
//   variant storage   //
/////////////////////////

/// @brief A class which determines size and alignment for a variant.
/// @note This has no implementation since it's instantiated only when no template parameter is given, i.e., invalid.
template <typename... Types>
struct variant_storage;

/// @note This can be either the first or an intermediate of chained template instantiations.
template <typename First, typename... Rest>
struct variant_storage<First, Rest...> : variant_storage<Rest...>
{
    using base_t = variant_storage<Rest...>;
    static constexpr std::size_t size = sizeof(First) > base_t::size ? sizeof(First) : base_t::size;
    static constexpr std::size_t align = alignof(First) > base_t::align ? alignof(First) : base_t::align;
};

/// @note This can be either the first or an intermediate of chained template instantiations.
template <typename PtrType, typename Second, typename... Rest>
struct variant_storage<heap_allocate_tag<PtrType>, Second, Rest...> : variant_storage<Second, Rest...>
{
    using base_t = variant_storage<Second, Rest...>;
    static constexpr std::size_t size = sizeof(PtrType) > base_t::size ? sizeof(PtrType) : base_t::size;
    static constexpr std::size_t align = alignof(PtrType) > base_t::align ? alignof(PtrType) : base_t::align;
};

/// @note This can be eigher the only or the last instantiation.
template <typename T>
struct variant_storage<T>
{
    static constexpr std::size_t size = sizeof(T);
    static constexpr std::size_t align = alignof(T);
};

/// @note This can be eigher the only or the last instantiation.
template <typename PtrType>
struct variant_storage<heap_allocate_tag<PtrType>>
{
    static constexpr std::size_t size = sizeof(PtrType);
    static constexpr std::size_t align = alignof(PtrType);
};

/// @brief An alias type of the aligned storage for a variant.
/// @todo
/// `std::aligned_storage` is deprecated in C++23.
/// Create a compile switch to get the storage type using `alignas(T) std::byte[sizeof(T)]`
template <typename... Types>
using aligned_variant_storage_t = aligned_storage_t<variant_storage<Types...>::size, variant_storage<Types...>::align>;

//////////////////////////////////////
//   variant manipulation helpers   //
//////////////////////////////////////

//
// create_alt_helper
//

/// @brief A helper class to create an alternative in the variant storage.
/// @tparam T The type of an alternative to be created.
template <typename T>
struct create_alt_helper
{
    /// @brief Creates an alternative with the given parameter and returns the id for the created object type.
    /// @tparam ...Args The arguments to be passed for construction.
    /// @param p_storage A pointer to the storage for the created alternative.
    /// @param ...args A parameter passed when creating an alternative.
    template <typename... Args, enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
    static void create(void* p_storage, Args&&... args)
    {
        ::new (p_storage) T(std::forward<Args>(args)...);
    }
};

/// @brief The specialization in case the target type is `std::nullptr_t`.
template <>
struct create_alt_helper<std::nullptr_t>
{
    /// @brief Creates a null alternative in the storage.
    /// @param p_storage A pointer to the storage for the created alternative.
    /// @param _ A dummy parameter for creation (unused)
    static void create(void* p_storage, std::nullptr_t /*unused*/)
    {
        *reinterpret_cast<std::nullptr_t*>(p_storage) = nullptr;
    }
};

//
// destroy_alt_helper
//

/// @brief A helper class to destroy an alternative in the storage.
/// @tparam ...Types The possible alternative types.
template <typename... Types>
struct destroy_alt_helper;

/// @brief The partial specialization in case 1 or more template paramters are given.
/// @tparam First The first type of the given template parameters.
/// @tparam ...Rest The rest of the given template parameters.
template <typename First, typename... Rest>
struct destroy_alt_helper<First, Rest...>
{
    /// @brief Destroys an alternative if `id` and `cur_id` are the same, delegate for the next call otherwise.
    /// @param id The id for the target alternative type.
    /// @param cur_id The id for the current alternative type.
    /// @param p_storage The pointer to the storage.
    static void destroy(std::size_t id, std::size_t cur_id, void* p_storage)
    {
        if (id == cur_id)
        {
            reinterpret_cast<First*>(p_storage)->~First();
        }
        else
        {
            destroy_alt_helper<Rest...>::destroy(id, cur_id + 1, p_storage);
        }
    }
};

/// @brief The specialization in case no template parameter is given.
template <>
struct destroy_alt_helper<>
{
    /// @brief Do nothing because something wrong is happening when this function is called.
    /// @param _ A dummy id for the target alternative type. (unused)
    /// @param _ A dummy id for the current alternative type. (unused)
    /// @param _ A dummy pointer to the storage. (unused)
    static void destroy(std::size_t /*unused*/, std::size_t /*unused*/, void* /*unused*/)
    {
    }
};

//
// move_alt_helper
//

/// @brief A helper class to move from the source alternative value to the destination.
/// @tparam T The type of an alternative to be moved.
template <typename T>
struct move_alt_helper
{
    /// @brief Moves an alternative value from the old storage to the new storage.
    static void move(void* p_old_storage, void* p_new_storage)
    {
        new (p_new_storage) T(std::move(*reinterpret_cast<T*>(p_old_storage)));
    }
};

/// @brief The specialization in case the target type is `std::nullptr_t`.
template <>
struct move_alt_helper<std::nullptr_t>
{
    /// @brief Assigns nullptr to the new storage.
    /// @param _ A dummy pointer to the old storage.
    /// @param p_new_storage The pointer to the new storage.
    static void move(void* /*unused*/, void* p_new_storage)
    {
        *reinterpret_cast<std::nullptr_t*>(p_new_storage) = nullptr;
    }
};

//
// copy_alt_helper
//

/// @brief A helper class to copy the source alternative value to the destination at runtime.
/// @tparam T The type of an alternative to be copied.
template <typename T>
struct copy_alt_helper
{
    /// @brief Copies the source alternative value to the destination storage.
    /// @param p_old_storage The pointer to the source storage.
    /// @param p_new_storage The pointer to the destination storage.
    static void copy(void* p_old_storage, void* p_new_storage)
    {
        new (p_new_storage) T((*reinterpret_cast<const T*>(p_old_storage)));
    }
};

/// @brief The specialization in case the target type is `std::nullptr_t`.
template <>
struct copy_alt_helper<std::nullptr_t>
{
    /// @brief Assigns nullptr to the new storage.
    static void copy(std::size_t id, void* p_old_storage, void* p_new_storage)
    {
        *reinterpret_cast<std::nullptr_t*>(p_new_storage) = nullptr;
    }
};

//
// get_alt_helper
//

/// @brief A helper class to get the value of a variant.
/// @tparam T The type of an alternative the reference to which is to be acquired.
template <typename T>
struct get_alt_helper
{
    /// @brief Gets the reference to the target alternative value.
    /// @param p_storage The pointer to the storage.
    static T& get(void* p_storage)
    {
        return *reinterpret_cast<remove_reference_t<T>*>(p_storage);
    }

    /// @brief Gets the constant reference to the target alternative value.
    /// @param p_storage The pointer to the storage.
    static const T& get(const void* p_storage)
    {
        return *reinterpret_cast<const remove_reference_t<T>*>(p_storage);
    }
};

/////////////////
//   variant   //
/////////////////

template <typename... Types>
class variant
{
public:
    // variant must not be empty until destruction.
    variant() = delete;

    /// @brief Creates a variant object with the given parameter.
    /// @tparam T The type of an argument passed for construction.
    /// @param t The argument passed for construction.
    template <typename T>
    explicit variant(T&& t)
        : id(get_type_id_helper<T, Types...>::value)
    {
        create_alt_helper<T>::create(reinterpret_cast<void*>(&storage), std::forward<T>(t));
    }

    /// @brief Copy constructor of the variant class.
    /// @param other A variant object to be copied with.
    variant(const variant& other)
        : id(other.id)
    {
        copy_alt_helper<Types...>::copy(id, reinterpret_cast<void*>(&storage), reinterpret_cast<void*>(&other.storage));
    }

    /// @brief Move constructor of the variant class.
    /// @param other A variant object to be moved from.
    variant(variant&& other)
        : id(other.id)
    {
        move_alt_helper<Types...>::move(id, reinterpret_cast<void*>(&storage), reinterpret_cast<void*>(&other.storage));
    }

    /// @brief Destroys a variant object and the current alternative.
    ~variant()
    {
        destroy_alt_helper<Types...>::destroy(id, 0, reinterpret_cast<void*>(&storage));
    }

    /// @brief Copy assignment operator of the variant class.
    /// @param other A variant object to be copied with.
    /// @return Reference to this variant object.
    variant& operator=(const variant& other)
    {
        variant(other).swap(*this);
        return *this;
    }

    /// @brief Move assignment operator of the variant class.
    /// @param other A variant object to be moved from.
    /// @return Reference to this variant object.
    variant& operator=(variant&& other)
    {
        variant(std::move(other)).swap(*this);
        return *this;
    }

    /// @brief Gets the id of the current alternative type.
    /// @return The id of the current alternative type.
    std::size_t index() const noexcept
    {
        return id;
    }

    /// @brief Get the pointer to the storage.
    /// @return The pointer to the storage.
    void* data() noexcept
    {
        return reinterpret_cast<void*>(&storage);
    }

    /// @brief Get the pointer to the storage.
    /// @return The pointer to the storage.
    const void* data() const noexcept
    {
        return reinterpret_cast<const void*>(&storage);
    }

    /// @brief Swaps the internally stored data with the given variant object.
    /// @param other A variant object to be swapped with.
    void swap(variant& other)
    {
        using std::swap;
        swap(id, other.id);

        decltype(variant::storage) tmp_storage {};
        std::memcpy(&tmp_storage, &storage, sizeof(variant::storage));
        std::memcpy(&storage, &other.storage, sizeof(variant::storage));
        std::memcpy(&other.storage, &tmp_storage, sizeof(variant::storage));
    }

private:
    /// The id of the current alternative type.
    std::size_t id;
    /// The storage for the value of an alternative.
    aligned_variant_storage_t<Types...> storage {};
};

//////////////////////////////////////////////
//   type getter for variant alternatives   //
//////////////////////////////////////////////

/// @brief A helper class which provides compile-time indexed access to the types of the alternatives.
/// @tparam N The index of the target alternative type.
/// @tparam ...Types The possible alternative types.
template <std::size_t N, typename... Types>
struct variant_alternative;

/// @brief The partial specialization in case 1 or more possible alternatives exist.
/// @tparam N The current index while looking for the target type recursively. This value will be decreased to 0.
/// @tparam First The first type of the possible alternaltive types left.
/// @tparam ...Rest The rest of the possible alternative types left.
template <std::size_t N, typename First, typename... Rest>
struct variant_alternative<N, variant<First, Rest...>> : variant_alternative<N - 1, variant<Rest...>>
{
};

/// @brief The partial specialization in case the index becomes 0.
/// @tparam First The first type of the possible alternative types left.
/// @tparam ...Rest The rest of the possible alternative types left.
template <typename First, typename... Rest>
struct variant_alternative<0, variant<First, Rest...>>
{
    using type = First;
};

/// @brief An alias type of the alternative type at the given index.
template <std::size_t N, typename... Types>
using variant_alternative_t = typename variant_alternative<N, variant<Types...>>::type;

/// @brief The partial specialization in case the target variant is a constant object.
template <std::size_t N, typename VariantType>
struct variant_alternative<N, const VariantType>
{
    using type = typename std::add_const<variant_alternative_t<N, VariantType>>::type;
};

///////////////////////////////////////////
//   non-member functions for variants   //
///////////////////////////////////////////

/// @brief Gets the reference to an alternative of the given type T from a variant.
/// @tparam T The target type.
/// @tparam ...Types The possible alternative types.
/// @param variant A variant object.
/// @return Reference to an alternative of the given type.
template <typename T, typename... Types>
constexpr T& get(variant<Types...>& variant)
{
    static_assert(!has_duplicate_types<Types...>::value, "T must occur once in variant alternatives.");
    static_assert(!std::is_void<T>::value, "T must not be void.");
    static_assert(get_type_id_helper<T, Types...>::value != sizeof...(Types), "T is not found in Types");

    return get<get_type_id_helper<T, Types...>::value>(variant);
}

/// @brief Gets the constant reference to an alternative of the given type T from a variant.
/// @tparam T The target type.
/// @tparam ...Types The possible alternative types.
/// @param variant A variant object.
/// @return Constant reference to an alternative of the given type.
template <typename T, typename... Types>
constexpr const T& get(const variant<Types...>& variant)
{
    static_assert(!has_duplicate_types<Types...>::value, "T must occur once in variant alternatives.");
    static_assert(!std::is_void<T>::value, "T must not be void.");
    static_assert(get_type_id_helper<T, Types...>::value != sizeof...(Types), "T is not found in Types");

    return get<get_type_id_helper<T, Types...>::value>(variant);
}

/// @brief Gets the reference to an alternative of the given index N from a variant.
/// @tparam N The target index.
/// @tparam ...Types The possible alternative types.
/// @param variant A variant object.
/// @return Reference to an alternative at the given index.
template <std::size_t N, typename... Types>
constexpr variant_alternative_t<N, Types...>& get(variant<Types...>& v)
{
    if (v.id != N)
    {
        throw fkyaml::type_error("invalid variant access with index.", N);
    }
    return get_alt_helper<variant_alternative_t<N, Types...>&>::get(v.data());
}

/// @brief Gets the constant reference to an alternative of the given index N from a variant.
/// @tparam N The target index.
/// @tparam ...Types The possible alternative types.
/// @param variant A variant object.
/// @return Constant reference to an alternative at the given index.
template <std::size_t N, typename... Types>
constexpr const variant_alternative_t<N, Types...>& get(const variant<Types...>& v)
{
    if (v.id != N)
    {
        throw fkyaml::type_error("invalid variant access with index.", N);
    }
    return get_alt_helper<const variant_alternative_t<N, Types...>&>::get(v.data());
}

/// @brief Applies the visitor to the variant.
/// @tparam Visitor The type of the visitor.
/// @tparam ...Types The possible alternative types.
/// @param visitor A callable object that accepts every possible alternative from the variant.
/// @param variant The target variant object.
template <typename Visitor, typename... Types>
constexpr void visit(Visitor&& visitor, variant<Types...>& variant)
{
}

/// @brief Applies the visitor to the variant.
/// @tparam Visitor The type of the visitor.
/// @tparam ...Types The possible alternative types.
/// @param visitor A callable object that accepts every possible alternative from the variant.
/// @param variant The target variant object.
template <typename Visitor, typename... Types>
constexpr void visit(Visitor&& visitor, const variant<Types...>& variant)
{
}

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_VARIANT_HPP_ */
