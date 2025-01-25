//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_EXCEPTION_SAFE_ALLOCATION_HPP
#define FK_YAML_DETAIL_EXCEPTION_SAFE_ALLOCATION_HPP

#include <memory>
#include <utility>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/assert.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Helper struct which ensures destruction/deallocation of heap-allocated objects.
/// @tparam ObjT Object type.
/// @tparam AllocTraits Allocator traits type for the object.
template <typename ObjT, typename AllocTraits>
struct tidy_guard {
    tidy_guard() = delete;

    /// @brief Construct a tidy_guard with a pointer to the object.
    /// @param p_obj
    tidy_guard(ObjT* p_obj) noexcept
        : p_obj(p_obj) {
    }

    // move-only
    tidy_guard(const tidy_guard&) = delete;
    tidy_guard& operator=(const tidy_guard&) = delete;

    /// @brief Move constructs a tidy_guard object.
    tidy_guard(tidy_guard&&) = default;

    /// @brief Move assigns a tidy_guard object.
    /// @return Reference to this tidy_guard object.
    tidy_guard& operator=(tidy_guard&&) = default;

    /// @brief Destroys this tidy_guard object. Destruction/deallocation happen if the pointer is not null.
    ~tidy_guard() {
        if FK_YAML_UNLIKELY (p_obj != nullptr) {
            typename AllocTraits::allocator_type alloc {};
            AllocTraits::destroy(alloc, p_obj);
            AllocTraits::deallocate(alloc, p_obj, 1);
            p_obj = nullptr;
        }
    }

    /// @brief Get the pointer to the object.
    /// @return The pointer to the object.
    ObjT* get() const noexcept {
        return p_obj;
    }

    /// @brief Checks if the pointer is not null.
    explicit operator bool() const noexcept {
        return p_obj != nullptr;
    }

    /// @brief Releases the pointer to the object. No destruction/deallocation happen after this function gets called.
    /// @return The pointer to the object.
    ObjT* release() noexcept {
        ObjT* ret = p_obj;
        p_obj = nullptr;
        return ret;
    }

    /// @brief The pointer to the object.
    ObjT* p_obj {nullptr};
};

/// @brief Allocates and constructs an `ObjT` object with given arguments.
/// @tparam ObjT The object type.
/// @tparam ...Args The argument types.
/// @param ...args The arguments for construction.
/// @return An address of allocated memory on the heap.
template <typename ObjT, typename... Args>
inline ObjT* create_object(Args&&... args) {
    using alloc_type = std::allocator<ObjT>;
    using alloc_traits_type = std::allocator_traits<alloc_type>;

    alloc_type alloc {};
    tidy_guard<ObjT, alloc_traits_type> tg {alloc_traits_type::allocate(alloc, 1)};
    alloc_traits_type::construct(alloc, tg.get(), std::forward<Args>(args)...);

    FK_YAML_ASSERT(tg);
    return tg.release();
}

/// @brief Destroys and deallocates an `ObjT` object.
/// @tparam ObjT The object type.
/// @param p_obj A pointer to the object.
template <typename ObjT>
inline void destroy_object(ObjT* p_obj) {
    FK_YAML_ASSERT(p_obj != nullptr);
    std::allocator<ObjT> alloc;
    std::allocator_traits<decltype(alloc)>::destroy(alloc, p_obj);
    std::allocator_traits<decltype(alloc)>::deallocate(alloc, p_obj, 1);
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_EXCEPTION_SAFE_ALLOCATION_HPP */
