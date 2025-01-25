//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_STR_VIEW_HPP
#define FK_YAML_DETAIL_STR_VIEW_HPP

#include <limits>
#include <string>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Non owning view into constant character sequence.
/// @note
/// This class is a minimal implementation of std::basic_string_view which has been available since C++17
/// but pretty useful and efficient for referencing/investigating character sequences.
/// @warning
/// This class intentionally omits a lot of value checks to improve efficiency. Necessary checks should be
/// made before calling this class' APIs for safety.
/// @tparam CharT Character type
/// @tparam Traits Character traits type which defaults to std::char_traits<CharT>.
template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_str_view {
    static_assert(!std::is_array<CharT>::value, "CharT must not be an array type.");
    static_assert(
        std::is_trivial<CharT>::value && std::is_standard_layout<CharT>::value,
        "CharT must be a trivial, standard layout type.");
    static_assert(
        std::is_same<CharT, typename Traits::char_type>::value, "CharT & Traits::char_type must be the same type.");

public:
    /// Character traits type.
    using traits_type = Traits;
    /// Character type.
    using value_type = CharT;
    /// Pointer type to a character.
    using pointer = value_type*;
    /// Constant pointer type to a character.
    using const_pointer = const value_type*;
    /// Reference type to a character.
    using reference = value_type&;
    /// Constant reference type to a character.
    using const_reference = const value_type&;
    /// Constant iterator type to a character.
    using const_iterator = const value_type*;
    /// Iterator type to a character.
    /// (Always constant since this class isn't meant to provide any mutating features.)
    using iterator = const_iterator;
    /// Constant reverse iterator type to a character.
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    /// Reverse iterator type to a character.
    /// (Always constant since this class isn't meant to provide any mutating features.)
    using reverse_iterator = const_reverse_iterator;
    /// Size type for character sequence sizes.
    using size_type = std::size_t;
    /// Difference type for distances between characters.
    using difference_type = std::ptrdiff_t;

    /// Invalid position value.
    static constexpr size_type npos = static_cast<size_type>(-1);

    /// Constructs a basic_str_view object.
    constexpr basic_str_view() noexcept = default;

    /// Destroys a basic_str_view object.
    ~basic_str_view() noexcept = default;

    /// @brief Copy constructs a basic_str_view object.
    /// @param _ A basic_str_view object to copy from.
    constexpr basic_str_view(const basic_str_view&) noexcept = default;

    /// @brief Move constructs a basic_str_view object.
    /// @param _ A basic_str_view object to move from.
    constexpr basic_str_view(basic_str_view&&) noexcept = default;

    /// @brief Constructs a basic_str_view object from a pointer to a character sequence.
    /// @note std::char_traits::length() is constexpr from C++17.
    /// @param p_str A pointer to a character sequence. (Must be null-terminated, or an undefined behavior.)
    template <
        typename CharPtrT,
        enable_if_t<
            conjunction<
                negation<std::is_array<CharPtrT>>, std::is_pointer<CharPtrT>,
                disjunction<std::is_same<CharPtrT, value_type*>, std::is_same<CharPtrT, const value_type*>>>::value,
            int> = 0>
    FK_YAML_CXX17_CONSTEXPR basic_str_view(CharPtrT p_str) noexcept
        : m_len(traits_type::length(p_str)),
          mp_str(p_str) {
    }

    /// @brief Constructs a basic_str_view object from a C-style char array.
    /// @note
    /// This constructor assumes the last element is the null character ('\0'). If that's not desirable, consider using
    /// one of the other overloads.
    /// @tparam N The size of a C-style char array.
    /// @param str A C-style char array. (Must be null-terminated)
    template <std::size_t N>
    constexpr basic_str_view(const value_type (&str)[N]) noexcept
        : m_len(N - 1),
          mp_str(&str[0]) {
    }

    /// @brief Construction from a null pointer is forbidden.
    basic_str_view(std::nullptr_t) = delete;

    /// @brief Constructs a basic_str_view object from a pointer to a character sequence and its size.
    /// @param p_str A pointer to a character sequence. (May or may not be null-terminated.)
    /// @param len The length of a character sequence.
    constexpr basic_str_view(const value_type* p_str, size_type len) noexcept
        : m_len(len),
          mp_str(p_str) {
    }

    /// @brief Constructs a basic_str_view object from compatible begin/end iterators
    /// @tparam ItrType Iterator type to a character.
    /// @param first The iterator to the first element of a character sequence.
    /// @param last The iterator to the past-the-end of a character sequence.
    template <
        typename ItrType,
        enable_if_t<
            conjunction<
                is_iterator_of<ItrType, CharT>,
                std::is_base_of<
                    std::random_access_iterator_tag, typename std::iterator_traits<ItrType>::iterator_category>>::value,
            int> = 0>
    basic_str_view(ItrType first, ItrType last) noexcept
        : m_len(last - first),
          mp_str(&*first) {
    }

    /// @brief Constructs a basic_str_view object from a compatible std::basic_string object.
    /// @param str A compatible character sequence container.
    basic_str_view(const std::basic_string<CharT>& str) noexcept
        : m_len(str.length()),
          mp_str(str.data()) {
    }

    /// @brief Copy assignment operator for this basic_str_view class.
    /// @param _ A basic_str_view object to copy from.
    /// @return Reference to this basic_str_view object.
    basic_str_view& operator=(const basic_str_view&) noexcept = default;

    /// @brief Move assignment operator for this basic_str_view class.
    /// @param _ A basic_str_view object to move from.
    /// @return Reference to this basic_str_view object.
    basic_str_view& operator=(basic_str_view&&) noexcept = default;

    /// @brief Get the iterator to the first element. (Always constant)
    /// @return The iterator to the first element.
    const_iterator begin() const noexcept {
        return mp_str;
    }

    /// @brief Get the iterator to the past-the-end element. (Always constant)
    /// @return The iterator to the past-the-end element.
    const_iterator end() const noexcept {
        return mp_str + m_len;
    }

    /// @brief Get the iterator to the first element. (Always constant)
    /// @return The iterator to the first element.
    const_iterator cbegin() const noexcept {
        return mp_str;
    }

    /// @brief Get the iterator to the past-the-end element. (Always constant)
    /// @return The iterator to the past-the-end element.
    const_iterator cend() const noexcept {
        return mp_str + m_len;
    }

    /// @brief Get the iterator to the first element in the reverse order. (Always constant)
    /// @return The iterator to the first element in the reverse order.
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    /// @brief Get the iterator to the past-the-end element in the reverse order. (Always constant)
    /// @return The iterator to the past-the-end element in the reverse order.
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    /// @brief Get the iterator to the first element in the reverse order. (Always constant)
    /// @return The iterator to the first element in the reverse order.
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    /// @brief Get the iterator to the past-the-end element in the reverse order. (Always constant)
    /// @return The iterator to the past-the-end element in the reverse order.
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    /// @brief Get the size of the referenced character sequence.
    /// @return The size of the referenced character sequence.
    size_type size() const noexcept {
        return m_len;
    }

    /// @brief Get the size of the referenced character sequence.
    /// @return The size of the referenced character sequence.
    size_type length() const noexcept {
        return m_len;
    }

    /// @brief Get the maximum number of the character sequence size.
    /// @return The maximum number of the character sequence size.
    constexpr size_type max_size() const noexcept {
        return static_cast<size_type>(std::numeric_limits<difference_type>::max());
    }

    /// @brief Checks if the referenced character sequence is empty.
    /// @return true if empty, false otherwise.
    bool empty() const noexcept {
        return m_len == 0;
    }

    /// @brief Get the element at the given position.
    /// @param pos The position of the target element.
    /// @return The element at the given position.
    const_reference operator[](size_type pos) const noexcept {
        return *(mp_str + pos);
    }

    /// @brief Get the element at the given position with bounds checks.
    /// @warning Throws an fkyaml::out_of_range exception if the position exceeds the character sequence size.
    /// @param pos The position of the target element.
    /// @return The element at the given position.
    const_reference at(size_type pos) const {
        if FK_YAML_UNLIKELY (pos >= m_len) {
            throw fkyaml::out_of_range(static_cast<int>(pos));
        }
        return *(mp_str + pos);
    }

    /// @brief Get the first element.
    /// @return The first element.
    const_reference front() const noexcept {
        return *mp_str;
    }

    /// @brief Get the last element.
    /// @return The last element.
    const_reference back() const {
        return *(mp_str + m_len - 1);
    }

    /// @brief Get the pointer to the raw data of referenced character sequence.
    /// @return The pointer to the raw data of referenced character sequence.
    const_pointer data() const noexcept {
        return mp_str;
    }

    /// @brief Moves the beginning position by `n` elements.
    /// @param n The number of elements by which to move the beginning position.
    void remove_prefix(size_type n) noexcept {
        mp_str += n;
        m_len -= n;
    }

    /// @brief Shrinks the referenced character sequence from the last by `n` elements.
    /// @param n The number of elements by which to shrink the sequence from the last.
    void remove_suffix(size_type n) noexcept {
        m_len -= n;
    }

    /// @brief Swaps data with the given basic_str_view object.
    /// @param other A basic_str_view object to swap data with.
    void swap(basic_str_view& other) noexcept {
        auto tmp = *this;
        *this = other;
        other = tmp;
    }

    /// @brief Copys the referenced character sequence values from `pos` by `n` size.
    /// @warning Throws an fkyaml::out_of_range exception if the given `pos` is bigger than the length.
    /// @param p_str The pointer to a character sequence buffer for output.
    /// @param n The number of elements to write into `p_str`.
    /// @param pos The offset of the beginning position to copy values.
    /// @return The number of elements to be written into `p_str`.
    size_type copy(CharT* p_str, size_type n, size_type pos = 0) const {
        if FK_YAML_UNLIKELY (pos > m_len) {
            throw fkyaml::out_of_range(static_cast<int>(pos));
        }
        const size_type rlen = std::min(n, m_len - pos);
        traits_type::copy(p_str, mp_str + pos, rlen);
        return rlen;
    }

    /// @brief Constructs a sub basic_str_view object from `pos` by `n` size.
    /// @warning Throws an fkyaml::out_of_range exception if the given `pos` is bigger than the length.
    /// @param pos The offset of the beginning position.
    /// @param n The number of elements to the end of a new sub basic_str_view object.
    /// @return A newly created sub basic_str_view object.
    basic_str_view substr(size_type pos = 0, size_type n = npos) const {
        if FK_YAML_UNLIKELY (pos > m_len) {
            throw fkyaml::out_of_range(static_cast<int>(pos));
        }
        const size_type rlen = std::min(n, m_len - pos);
        return basic_str_view(mp_str + pos, rlen);
    }

    /// @brief Compares the referenced character sequence values with the given basic_str_view object.
    /// @param sv The basic_str_view object to compare with.
    /// @return The lexicographical comparison result. The values are same as std::strncmp().
    int compare(basic_str_view sv) const noexcept {
        const size_type rlen = std::min(m_len, sv.m_len);
        int ret = traits_type::compare(mp_str, sv.mp_str, rlen);

        if (ret == 0) {
            using int_limits = std::numeric_limits<int>;
            const difference_type diff =
                m_len > sv.m_len ? m_len - sv.m_len
                                 : static_cast<difference_type>(-1) * static_cast<difference_type>(sv.m_len - m_len);

            if (diff > int_limits::max()) {
                ret = int_limits::max();
            }
            else if (diff < int_limits::min()) {
                ret = int_limits::min();
            }
            else {
                ret = static_cast<int>(diff);
            }
        }

        return ret;
    }

    /// @brief Compares the referenced character sequence values from `pos1` by `n1` characters with `sv`.
    /// @param pos1 The offset of the beginning element.
    /// @param n1 The length of character sequence used for comparison.
    /// @param sv A basic_str_view object to compare with.
    /// @return The lexicographical comparison result. The values are same as std::strncmp().
    int compare(size_type pos1, size_type n1, basic_str_view sv) const {
        return substr(pos1, n1).compare(sv);
    }

    /// @brief Compares the referenced character sequence value from `pos1` by `n1` characters with `sv` from `pos2` by
    /// `n2` characters.
    /// @param pos1 The offset of the beginning element in this character sequence.
    /// @param n1 The length of this character sequence used for comparison.
    /// @param sv A basic_str_view object to compare with.
    /// @param pos2 The offset of the beginning element in `sv`.
    /// @param n2 The length of `sv` used for comparison.
    /// @return The lexicographical comparison result. The values are same as std::strncmp().
    int compare(size_type pos1, size_type n1, basic_str_view sv, size_type pos2, size_type n2) const {
        return substr(pos1, n1).compare(sv.substr(pos2, n2));
    }

    /// @brief Compares the referenced character sequence with `s` character sequence.
    /// @param s The pointer to a character sequence to compare with.
    /// @return The lexicographical comparison result. The values are same as std::strncmp().
    int compare(const CharT* s) const {
        return compare(basic_str_view(s));
    }

    /// @brief Compares the referenced character sequence from `pos1` by `n1` characters with `s` character sequence.
    /// @param pos1 The offset of the beginning element in this character sequence.
    /// @param n1 The length of this character sequence used fo comparison.
    /// @param s The pointer to a character sequence to compare with.
    /// @return The lexicographical comparison result. The values are same as std::strncmp().
    int compare(size_type pos1, size_type n1, const CharT* s) const {
        return substr(pos1, n1).compare(basic_str_view(s));
    }

    /// @brief Compares the referenced character sequence from `pos1` by `n1` characters with `s` character sequence by
    /// `n2` characters.
    /// @param pos1 The offset of the beginning element in this character sequence.
    /// @param n1 The length of this character sequence used fo comparison.
    /// @param s The pointer to a character sequence to compare with.
    /// @param n2 The length of `s` used fo comparison.
    /// @return
    int compare(size_type pos1, size_type n1, const CharT* s, size_type n2) const {
        return substr(pos1, n1).compare(basic_str_view(s, n2));
    }

    /// @brief Checks if this character sequence starts with `sv` characters.
    /// @param sv The character sequence to compare with.
    /// @return true if the character sequence starts with `sv` characters, false otherwise.
    bool starts_with(basic_str_view sv) const {
        return substr(0, sv.size()) == sv;
    }

    /// @brief Checks if this character sequence starts with `c` character.
    /// @param c The character to compare with.
    /// @return true if the character sequence starts with `c` character, false otherwise.
    bool starts_with(CharT c) const noexcept {
        return !empty() && traits_type::eq(front(), c);
    }

    /// @brief Checks if this character sequence starts with `s` characters.
    /// @param s The character sequence to compare with.
    /// @return true if the character sequence starts with `s` characters, false otherwise.
    bool starts_with(const CharT* s) const {
        return starts_with(basic_str_view(s));
    }

    /// @brief Checks if this character sequence ends with `sv` characters.
    /// @param sv The character sequence to compare with.
    /// @return true if the character sequence ends with `sv` characters, false otherwise.
    bool ends_with(basic_str_view sv) const noexcept {
        const size_type size = m_len;
        const size_type sv_size = sv.size();
        return size >= sv_size && traits_type::compare(end() - sv_size, sv.data(), sv_size) == 0;
    }

    /// @brief Checks if this character sequence ends with `c` character.
    /// @param c The character to compare with.
    /// @return true if the character sequence ends with `c` character, false otherwise.
    bool ends_with(CharT c) const noexcept {
        return !empty() && traits_type::eq(back(), c);
    }

    /// @brief Checks if this character sequence ends with `s` characters.
    /// @param s The character sequence to compare with.
    /// @return true if the character sequence ends with `s` characters, false otherwise.
    bool ends_with(const CharT* s) const noexcept {
        return ends_with(basic_str_view(s));
    }

    /// @brief Checks if this character sequence contains `sv` characters.
    /// @param sv The character sequence to compare with.
    /// @return true if the character sequence contains `sv` characters, false otherwise.
    bool contains(basic_str_view sv) const noexcept {
        return find(sv) != npos;
    }

    /// @brief Checks if this character sequence contains `c` character.
    /// @param c The character to compare with.
    /// @return true if the character sequence contains `c` character, false otherwise.
    bool contains(CharT c) const noexcept {
        return find(c) != npos;
    }

    /// @brief Checks if this character sequence contains `s` characters.
    /// @param s The character sequence to compare with.
    /// @return true if the character sequence contains `s` characters, false otherwise.
    bool contains(const CharT* s) const noexcept {
        return find(s) != npos;
    }

    /// @brief Finds the beginning position of `sv` characters in this referenced character sequence.
    /// @param sv The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of `sv` characters, `npos` otherwise.
    size_type find(basic_str_view sv, size_type pos = 0) const noexcept {
        return find(sv.mp_str, pos, sv.m_len);
    }

    /// @brief Finds the beginning position of `c` character in this referenced character sequence.
    /// @param sv The character to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of `c` character, `npos` otherwise.
    size_type find(CharT c, size_type pos = 0) const noexcept {
        size_type ret = npos;

        if FK_YAML_LIKELY (pos < m_len) {
            const size_type n = m_len - pos;
            const CharT* p_found = traits_type::find(mp_str + pos, n, c);
            if (p_found) {
                ret = p_found - mp_str;
            }
        }

        return ret;
    }

    /// @brief Finds the beginning position of `s` character sequence by `n` characters in this referenced character
    /// sequence.
    /// @param s The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @param n The length of `s` character sequence used for comparison.
    /// @return The beginning position of `s` characters, `npos` otherwise.
    size_type find(const CharT* s, size_type pos, size_type n) const noexcept {
        if FK_YAML_UNLIKELY (n == 0) {
            return pos <= m_len ? pos : npos;
        }

        if FK_YAML_UNLIKELY (pos >= m_len) {
            return npos;
        }

        CharT s0 = s[0];
        const CharT* p_first = mp_str + pos;
        const CharT* p_last = mp_str + m_len;
        size_type len = m_len - pos;

        while (len >= n) {
            // find the first occurence of s0
            p_first = traits_type::find(p_first, len - n + 1, s0);
            if (!p_first) {
                return npos;
            }

            // compare the full strings from the first occurence of s0
            if (traits_type::compare(p_first, s, n) == 0) {
                return p_first - mp_str;
            }

            len = p_last - (++p_first);
        }

        return npos;
    }

    /// @brief Finds the beginning position of `s` character sequence in this referenced character sequence.
    /// @param s The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of `s` characters, `npos` otherwise.
    size_type find(const CharT* s, size_type pos = 0) const noexcept {
        return find(basic_str_view(s), pos);
    }

    /// @brief Retrospectively finds the beginning position of `sv` characters in this referenced character sequence.
    /// @param sv The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of `sv` characters, `npos` otherwise.
    size_type rfind(basic_str_view sv, size_type pos = npos) const noexcept {
        return rfind(sv.mp_str, pos, sv.m_len);
    }

    /// @brief Retrospectively finds the beginning position of `c` character in this referenced character sequence.
    /// @param sv The character to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of `c` character, `npos` otherwise.
    size_type rfind(CharT c, size_type pos = npos) const noexcept {
        if FK_YAML_UNLIKELY (m_len == 0) {
            return npos;
        }

        const size_type idx = std::min(m_len - 1, pos);

        for (size_type i = 0; i <= idx; i++) {
            if (traits_type::eq(mp_str[idx - i], c)) {
                return idx - i;
            }
        }

        return npos;
    }

    /// @brief Retrospectively finds the beginning position of `s` character sequence by `n` characters in this
    /// referenced character sequence.
    /// @param s The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @param n The length of `s` character sequence used for comparison.
    /// @return The beginning position of `s` characters, `npos` otherwise.
    size_type rfind(const CharT* s, size_type pos, size_type n) const noexcept {
        if FK_YAML_LIKELY (n <= m_len) {
            pos = std::min(m_len - n, pos) + 1;

            do {
                if (traits_type::compare(mp_str + --pos, s, n) == 0) {
                    return pos;
                }
            } while (pos > 0);
        }

        return npos;
    }

    /// @brief Retrospectively finds the beginning position of `s` character sequence in this referenced character
    /// sequence.
    /// @param s The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of `s` characters, `npos` otherwise.
    size_type rfind(const CharT* s, size_type pos = npos) const noexcept {
        return rfind(basic_str_view(s), pos);
    }

    /// @brief Finds the first occurence of `sv` character sequence in this referenced character sequence.
    /// @param sv The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of `sv` characters, `npos` otherwise.
    size_type find_first_of(basic_str_view sv, size_type pos = 0) const noexcept {
        return find_first_of(sv.mp_str, pos, sv.m_len);
    }

    /// @brief Finds the first occurence of `c` character in this referenced character sequence.
    /// @param c The character to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of `c` character, `npos` otherwise.
    size_type find_first_of(CharT c, size_type pos = 0) const noexcept {
        return find(c, pos);
    }

    /// @brief Finds the first occurence of `s` character sequence by `n` characters in this referenced character
    /// sequence.
    /// @param s The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @param n The length of `s` character sequence used for comparison.
    /// @return The beginning position of `s` characters, `npos` otherwise.
    size_type find_first_of(const CharT* s, size_type pos, size_type n) const noexcept {
        if FK_YAML_UNLIKELY (n == 0) {
            return npos;
        }

        for (size_type idx = pos; idx < m_len; ++idx) {
            const CharT* p_found = traits_type::find(s, n, mp_str[idx]);
            if (p_found) {
                return idx;
            }
        }

        return npos;
    }

    /// @brief Finds the first occurence of `s` character sequence in this referenced character sequence.
    /// @param s The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of `s` characters, `npos` otherwise.
    size_type find_first_of(const CharT* s, size_type pos = 0) const noexcept {
        return find_first_of(basic_str_view(s), pos);
    }

    /// @brief Finds the last occurence of `sv` character sequence in this referenced character sequence.
    /// @param sv The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of `sv` characters, `npos` otherwise.
    size_type find_last_of(basic_str_view sv, size_type pos = npos) const noexcept {
        return find_last_of(sv.mp_str, pos, sv.m_len);
    }

    /// @brief Finds the last occurence of `c` character in this referenced character sequence.
    /// @param c The character to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of `c` character, `npos` otherwise.
    size_type find_last_of(CharT c, size_type pos = npos) const noexcept {
        return rfind(c, pos);
    }

    /// @brief Finds the last occurence of `s` character sequence by `n` characters in this referenced character
    /// sequence.
    /// @param s The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @param n The length of `s` character sequence used for comparison.
    /// @return The beginning position of `s` characters, `npos` otherwise.
    size_type find_last_of(const CharT* s, size_type pos, size_type n) const noexcept {
        if FK_YAML_LIKELY (n <= m_len) {
            pos = std::min(m_len - n - 1, pos);

            do {
                const CharT* p_found = traits_type::find(s, n, mp_str[pos]);
                if (p_found) {
                    return pos;
                }
            } while (pos-- != 0);
        }

        return npos;
    }

    /// @brief Finds the last occurence of `s` character sequence in this referenced character sequence.
    /// @param s The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of `s` characters, `npos` otherwise.
    size_type find_last_of(const CharT* s, size_type pos = npos) const noexcept {
        return find_last_of(basic_str_view(s), pos);
    }

    /// @brief Finds the first absence of `sv` character sequence in this referenced character sequence.
    /// @param sv The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of non `sv` characters, `npos` otherwise.
    size_type find_first_not_of(basic_str_view sv, size_type pos = 0) const noexcept {
        return find_first_not_of(sv.mp_str, pos, sv.m_len);
    }

    /// @brief Finds the first absence of `c` character in this referenced character sequence.
    /// @param c The character to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of non `c` character, `npos` otherwise.
    size_type find_first_not_of(CharT c, size_type pos = 0) const noexcept {
        for (; pos < m_len; ++pos) {
            if (!traits_type::eq(mp_str[pos], c)) {
                return pos;
            }
        }

        return npos;
    }

    /// @brief Finds the first absence of `s` character sequence by `n` characters in this referenced character
    /// sequence.
    /// @param s The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @param n The length of `s` character sequence used for comparison.
    /// @return The beginning position of non `s` characters, `npos` otherwise.
    size_type find_first_not_of(const CharT* s, size_type pos, size_type n) const noexcept {
        for (; pos < m_len; ++pos) {
            const CharT* p_found = traits_type::find(s, n, mp_str[pos]);
            if (!p_found) {
                return pos;
            }
        }

        return npos;
    }

    /// @brief Finds the first absence of `s` character sequence in this referenced character sequence.
    /// @param s The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of non `s` characters, `npos` otherwise.
    size_type find_first_not_of(const CharT* s, size_type pos = 0) const noexcept {
        return find_first_not_of(basic_str_view(s), pos);
    }

    /// @brief Finds the last absence of `sv` character sequence in this referenced character sequence.
    /// @param sv The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of non `sv` characters, `npos` otherwise.
    size_type find_last_not_of(basic_str_view sv, size_type pos = npos) const noexcept {
        return find_last_not_of(sv.mp_str, pos, sv.m_len);
    }

    /// @brief Finds the last absence of `c` character in this referenced character sequence.
    /// @param c The character to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of non `c` character, `npos` otherwise.
    size_type find_last_not_of(CharT c, size_type pos = npos) const noexcept {
        if FK_YAML_LIKELY (m_len > 0) {
            pos = std::min(m_len, pos);

            do {
                if (!traits_type::eq(mp_str[--pos], c)) {
                    return pos;
                }
            } while (pos > 0);
        }

        return npos;
    }

    /// @brief Finds the last absence of `s` character sequence by `n` characters in this referenced character
    /// sequence.
    /// @param s The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @param n The length of `s` character sequence used for comparison.
    /// @return The beginning position of non `s` characters, `npos` otherwise.
    size_type find_last_not_of(const CharT* s, size_type pos, size_type n) const noexcept {
        if FK_YAML_UNLIKELY (n <= m_len) {
            pos = std::min(m_len - n, pos) + 1;

            do {
                const CharT* p_found = traits_type::find(s, n, mp_str[--pos]);
                if (!p_found) {
                    return pos;
                }
            } while (pos > 0);
        }

        return npos;
    }

    /// @brief Finds the last absence of `s` character sequence in this referenced character sequence.
    /// @param s The character sequence to compare with.
    /// @param pos The offset of the search beginning position in this referenced character sequence.
    /// @return The beginning position of non `s` characters, `npos` otherwise.
    size_type find_last_not_of(const CharT* s, size_type pos = npos) const noexcept {
        return find_last_not_of(basic_str_view(s), pos);
    }

private:
    size_type m_len {0};
    const value_type* mp_str {nullptr};
};

// Prior to C++17, a static constexpr class member needs an out-of-class definition.
#ifndef FK_YAML_HAS_CXX_17

template <typename CharT, typename Traits>
constexpr typename basic_str_view<CharT, Traits>::size_type basic_str_view<CharT, Traits>::npos;

#endif // !defined(FK_YAML_HAS_CXX_17)

/// @brief An equal-to operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @param lhs A basic_str_view object for comparison.
/// @param rhs A basic_str_view object to compare with.
/// @return true if the two objects are the same, false otherwise.
template <typename CharT, typename Traits>
inline bool operator==(basic_str_view<CharT, Traits> lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    // Comparing the lengths first will omit unnecessary value comparison in compare().
    return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
}

/// @brief An equal-to operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @param lhs A basic_str_view object for comparison.
/// @param rhs A basic_string object to compare with.
/// @return true if the two objects are the same, false otherwise.
template <typename CharT, typename Traits>
inline bool operator==(basic_str_view<CharT, Traits> lhs, const std::basic_string<CharT, Traits>& rhs) noexcept {
    return lhs == basic_str_view<CharT, Traits>(rhs);
}

/// @brief An equal-to operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @param lhs A basic_string object for comparison.
/// @param rhs A basic_str_view object to compare with.
/// @return true if the two objects are the same, false otherwise.
template <typename CharT, typename Traits>
inline bool operator==(const std::basic_string<CharT, Traits>& lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    return basic_str_view<CharT, Traits>(lhs) == rhs;
}

/// @brief An equal-to operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @tparam N The length of the character array.
/// @param lhs A basic_str_view object for comparison.
/// @param rhs A character array to compare with.
/// @return true if the two objects are the same, false otherwise.
template <typename CharT, typename Traits, std::size_t N>
inline bool operator==(basic_str_view<CharT, Traits> lhs, const CharT (&rhs)[N]) noexcept {
    // assume `rhs` is null terminated
    return lhs == basic_str_view<CharT, Traits>(rhs);
}

/// @brief An equal-to operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @tparam N The length of the character array.
/// @param rhs A character array for comparison.
/// @param lhs A basic_str_view object to compare with.
/// @return true if the two objects are the same, false otherwise.
template <typename CharT, typename Traits, std::size_t N>
inline bool operator==(const CharT (&lhs)[N], basic_str_view<CharT, Traits> rhs) noexcept {
    // assume `lhs` is null terminated
    return basic_str_view<CharT, Traits>(lhs) == rhs;
}

/// @brief An not-equal-to operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @param lhs A basic_str_view object for comparison.
/// @param rhs A basic_str_view object to compare with.
/// @return true if the two objects are different, false otherwise.
template <typename CharT, typename Traits>
inline bool operator!=(basic_str_view<CharT, Traits> lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    return !(lhs == rhs);
}

/// @brief An not-equal-to operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @param lhs A basic_str_view object for comparison.
/// @param rhs A basic_string object to compare with.
/// @return true if the two objects are different, false otherwise.
template <typename CharT, typename Traits>
inline bool operator!=(basic_str_view<CharT, Traits> lhs, const std::basic_string<CharT, Traits>& rhs) noexcept {
    return !(lhs == basic_str_view<CharT, Traits>(rhs));
}

/// @brief An not-equal-to operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @param lhs A basic_string object for comparison.
/// @param rhs A basic_str_view object to compare with.
/// @return true if the two objects are different, false otherwise.
template <typename CharT, typename Traits>
inline bool operator!=(const std::basic_string<CharT, Traits>& lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    return !(basic_str_view<CharT, Traits>(lhs) == rhs);
}

/// @brief An not-equal-to operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @tparam N The length of the character array.
/// @param lhs A basic_str_view object for comparison.
/// @param rhs A character array to compare with.
/// @return true if the two objects are different, false otherwise.
template <typename CharT, typename Traits, std::size_t N>
inline bool operator!=(basic_str_view<CharT, Traits> lhs, const CharT (&rhs)[N]) noexcept {
    // assume `rhs` is null terminated.
    return !(lhs == basic_str_view<CharT, Traits>(rhs, N - 1));
}

/// @brief An not-equal-to operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @tparam N The length of the character array.
/// @param rhs A character array for comparison.
/// @param lhs A basic_str_view object to compare with.
/// @return true if the two objects are different, false otherwise.
template <typename CharT, typename Traits, std::size_t N>
inline bool operator!=(const CharT (&lhs)[N], basic_str_view<CharT, Traits> rhs) noexcept {
    // assume `lhs` is null terminate
    return !(basic_str_view<CharT, Traits>(lhs, N - 1) == rhs);
}

/// @brief An less-than operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @param lhs A basic_str_view object for comparison.
/// @param rhs A basic_str_view object to compare with.
/// @return true if `lhs` is less than `rhs`, false otherwise.
template <typename CharT, typename Traits>
inline bool operator<(basic_str_view<CharT, Traits> lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    return lhs.compare(rhs) < 0;
}

/// @brief An less-than-or-equal-to operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @param lhs A basic_str_view object for comparison.
/// @param rhs A basic_str_view object to compare with.
/// @return true if `lhs` is less than or equal to `rhs`, false otherwise.
template <typename CharT, typename Traits>
inline bool operator<=(basic_str_view<CharT, Traits> lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    return lhs.compare(rhs) <= 0;
}

/// @brief An greater-than operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @param lhs A basic_str_view object for comparison.
/// @param rhs A basic_str_view object to compare with.
/// @return true if `lhs` is greater than `rhs`, false otherwise.
template <typename CharT, typename Traits>
inline bool operator>(basic_str_view<CharT, Traits> lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    return lhs.compare(rhs) > 0;
}

/// @brief An greater-than-or-equal-to operator of the basic_str_view class.
/// @tparam CharT Character type
/// @tparam Traits Character traits type.
/// @param lhs A basic_str_view object for comparison.
/// @param rhs A basic_str_view object to compare with.
/// @return true if `lhs` is greater than or equal to `rhs`, false otherwise.
template <typename CharT, typename Traits>
inline bool operator>=(basic_str_view<CharT, Traits> lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    return lhs.compare(rhs) >= 0;
}

/// @brief Insertion operator of the basic_str_view class.
/// @tparam CharT Character type.
/// @tparam Traits Character traits type.
/// @param os An output stream object.
/// @param sv A basic_str_view object.
/// @return Reference to the output stream object `os`.
template <typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits>& operator<<(
    std::basic_ostream<CharT, Traits>& os, basic_str_view<CharT, Traits> sv) {
    return os.write(sv.data(), static_cast<std::streamsize>(sv.size()));
}

/// @brief view into `char` sequence.
using str_view = basic_str_view<char>;

#if FK_YAML_HAS_CHAR8_T
/// @brief view into `char8_t` sequence.
using u8str_view = basic_str_view<char8_t>;
#endif

/// @brief view into `char16_t` sequence.
using u16str_view = basic_str_view<char16_t>;

/// @brief view into `char32_t` sequence.
using u32str_view = basic_str_view<char32_t>;

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_STR_VIEW_HPP */
