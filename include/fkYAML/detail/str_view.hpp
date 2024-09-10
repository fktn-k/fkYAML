///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_STR_VIEW_HPP_
#define FK_YAML_DETAIL_STR_VIEW_HPP_

#include <limits>
#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_str_view {
    static_assert(!std::is_array<CharT>::value, "CharT must not be an array type.");
    static_assert(
        std::is_trivial<CharT>::value && std::is_standard_layout<CharT>::value,
        "CharT must be a trivial, standard layout type.");
    static_assert(
        std::is_same<CharT, typename Traits::char_type>::value, "CharT & Traits::char_type must be the same type.");

public:
    using traits_type = Traits;
    using value_type = CharT;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using const_iterator = const value_type*;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static constexpr size_type npos = static_cast<size_type>(-1);

    basic_str_view() noexcept = default;
    ~basic_str_view() noexcept = default;
    basic_str_view(const basic_str_view&) noexcept = default;
    basic_str_view(basic_str_view&&) noexcept = default;

    basic_str_view(const value_type* p_str) noexcept
        : m_len(traits_type::length(p_str)),
          mp_str(p_str) {
    }

    basic_str_view(std::nullptr_t) = delete;

    basic_str_view(const value_type* p_str, size_type len) noexcept
        : m_len(len),
          mp_str(p_str) {
    }

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
          mp_str(m_len > 0 ? &*first : nullptr) {
    }

    basic_str_view(const std::basic_string<CharT>& str) noexcept
        : m_len(str.length()),
          mp_str(str.data()) {
    }

    basic_str_view& operator=(const basic_str_view&) noexcept = default;
    basic_str_view& operator=(basic_str_view&&) noexcept = default;

    const_iterator begin() const noexcept {
        return mp_str;
    }

    const_iterator end() const noexcept {
        return mp_str + m_len;
    }

    const_iterator cbegin() const noexcept {
        return mp_str;
    }

    const_iterator cend() const noexcept {
        return mp_str + m_len;
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    size_type size() const noexcept {
        return m_len;
    }

    size_type length() const noexcept {
        return m_len;
    }

    constexpr size_type max_size() const noexcept {
        return std::numeric_limits<difference_type>::max();
    }

    bool empty() const noexcept {
        return m_len == 0;
    }

    const_reference operator[](size_type pos) const noexcept {
        return *(mp_str + pos);
    }

    const_reference at(size_type pos) const {
        if (pos >= m_len) {
            throw fkyaml::out_of_range(pos);
        }
        return *(mp_str + pos);
    }

    const_reference front() const noexcept {
        return *mp_str;
    }

    const_reference back() const {
        return *(mp_str + m_len - 1);
    }

    const_pointer data() const noexcept {
        return mp_str;
    }

    void remove_prefix(size_type n) noexcept {
        mp_str += n;
        m_len -= n;
    }

    void remove_suffix(size_type n) noexcept {
        m_len -= n;
    }

    void swap(basic_str_view& other) noexcept {
        auto tmp = *this;
        *this = other;
        other = tmp;
    }

    size_type copy(CharT* p_str, size_type n, size_type pos = 0) const {
        if (pos > m_len) {
            throw fkyaml::out_of_range(pos);
        }
        const size_type rlen = std::min(n, m_len - pos);
        traits_type::copy(p_str, mp_str + pos, rlen);
        return rlen;
    }

    basic_str_view substr(size_type pos = 0, size_type n = npos) const {
        if (pos > m_len) {
            throw fkyaml::out_of_range(pos);
        }
        const size_type rlen = std::min(n, m_len - pos);
        return basic_str_view(mp_str + pos, rlen);
    }

    int compare(basic_str_view sv) const noexcept {
        const size_type rlen = std::min(m_len, sv.m_len);
        int ret = traits_type::compare(mp_str, sv.mp_str, rlen);

        if (ret == 0) {
            using int_limits = std::numeric_limits<int>;
            difference_type diff = m_len - sv.m_len;

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

    int compare(size_type pos1, size_type n1, basic_str_view sv) const {
        return substr(pos1, n1).compare(sv);
    }

    int compare(size_type pos1, size_type n1, basic_str_view sv, size_type pos2, size_type n2) const {
        return substr(pos1, n1).compare(sv.substr(pos2, n2));
    }

    int compare(const CharT* s) const {
        return compare(basic_str_view(s));
    }

    int compare(size_type pos1, size_type n1, const CharT* s) const {
        return substr(pos1, n1).compare(basic_str_view(s));
    }

    int compare(size_type pos1, size_type n1, const CharT* s, size_type n2) const {
        return substr(pos1, n1).compare(basic_str_view(s, n2));
    }

    bool starts_with(basic_str_view sv) const noexcept {
        return substr(0, sv.size()) == sv;
    }

    bool starts_with(CharT c) const noexcept {
        return !empty() && traits_type::eq(front(), c);
    }

    bool starts_with(const CharT* s) const noexcept {
        return starts_with(basic_str_view(s));
    }

    bool ends_with(basic_str_view sv) const noexcept {
        const size_type size = m_len;
        const size_type sv_size = sv.size();
        return size >= sv_size && traits_type::compare(end() - sv_size, sv.data(), sv_size) == 0;
    }

    bool ends_with(CharT c) const noexcept {
        return !empty() && traits_type::eq(back(), c);
    }

    bool ends_with(const CharT* s) const noexcept {
        return ends_with(basic_str_view(s));
    }

    bool contains(basic_str_view sv) const noexcept {
        return find(sv) != npos;
    }

    bool contains(CharT c) const noexcept {
        return find(c) != npos;
    }

    bool contains(const CharT* s) const noexcept {
        return find(s) != npos;
    }

    size_type find(basic_str_view sv, size_type pos = 0) const noexcept {
        return find(sv.mp_str, pos, sv.m_len);
    }

    size_type find(CharT c, size_type pos = 0) const noexcept {
        size_type ret = npos;

        if (pos < m_len) {
            const size_type n = m_len - pos;
            const CharT* p_found = traits_type::find(mp_str + pos, n, c);
            if (p_found) {
                ret = p_found - mp_str;
            }
        }

        return ret;
    }

    size_type find(const CharT* s, size_type pos, size_type n) const noexcept {
        if (n == 0) {
            return pos <= m_len ? pos : npos;
        }

        if (pos >= m_len) {
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

    size_type find(const CharT* s, size_type pos = 0) const noexcept {
        return find(basic_str_view(s), pos);
    }

    size_type rfind(basic_str_view sv, size_type pos = npos) const noexcept {
        return rfind(sv.mp_str, pos, sv.m_len);
    }

    size_type rfind(CharT c, size_type pos = npos) const noexcept {
        if (m_len == 0) {
            return npos;
        }

        size_type idx = pos;
        if (pos >= m_len) {
            idx = m_len - 1;
        }

        while (idx > 0) {
            if (traits_type::eq(mp_str[idx], c)) {
                return idx;
            }
            --idx;
        }

        return npos;
    }

    size_type rfind(const CharT* s, size_type pos, size_type n) const noexcept {
        if (n <= m_len) {
            pos = std::min(m_len - n, pos) + 1;

            do {
                if (traits_type::compare(mp_str + --pos, s, n) == 0) {
                    return pos;
                }
            } while (pos > 0);
        }

        return npos;
    }

    size_type rfind(const CharT* s, size_type pos = npos) const noexcept {
        return rfind(basic_str_view(s), pos);
    }

    size_type find_first_of(basic_str_view sv, size_type pos = 0) const noexcept {
        return find_first_of(sv.mp_str, pos, sv.m_len);
    }

    size_type find_first_of(CharT c, size_type pos = 0) const noexcept {
        return find(c, pos);
    }

    size_type find_first_of(const CharT* s, size_type pos, size_type n) const noexcept {
        if (n == 0) {
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

    size_type find_first_of(const CharT* s, size_type pos = 0) const noexcept {
        return find_first_of(basic_str_view(s), pos);
    }

    size_type find_last_of(basic_str_view sv, size_type pos = npos) const noexcept {
        return find_last_of(sv.mp_str, pos, sv.m_len);
    }

    size_type find_last_of(CharT c, size_type pos = npos) const noexcept {
        return rfind(c, pos);
    }

    size_type find_last_of(const CharT* s, size_type pos, size_type n) const noexcept {
        if (n <= m_len) {
            pos = std::min(m_len - n, pos) + 1;

            do {
                const CharT* p_found = traits_type::find(s, n, mp_str[--pos]);
                if (p_found) {
                    return pos;
                }
            } while (pos > 0);
        }

        return npos;
    }

    size_type find_last_of(const CharT* s, size_type pos = npos) const noexcept {
        return find_last_of(basic_str_view(s), pos);
    }

    size_type find_first_not_of(basic_str_view sv, size_type pos = npos) const noexcept {
        return find_first_not_of(sv.mp_str, pos, sv.m_len);
    }

    size_type find_first_not_of(CharT c, size_type pos = npos) const noexcept {
        for (; pos < m_len; ++pos) {
            if (!traits_type::eq(mp_str[pos], c)) {
                return pos;
            }
        }

        return npos;
    }

    size_type find_first_not_of(const CharT* s, size_type pos, size_type n) const noexcept {
        for (; pos < m_len; ++pos) {
            const CharT* p_found = traits_type::find(s, n, mp_str[pos]);
            if (!p_found) {
                return pos;
            }
        }

        return npos;
    }

    size_type find_first_not_of(const CharT* s, size_type pos = npos) const noexcept {
        return find_first_not_of(basic_str_view(s), pos);
    }

    size_type find_last_not_of(basic_str_view sv, size_type pos = npos) const noexcept {
        return find_last_not_of(sv.mp_str, pos, sv.m_len);
    }

    size_type find_last_not_of(CharT c, size_type pos = npos) const noexcept {
        if (m_len > 0) {
            pos = std::min(m_len, pos);

            do {
                if (!traits_type::eq(mp_str[--pos], c)) {
                    return pos;
                }
            } while (pos > 0);
        }

        return npos;
    }

    size_type find_last_not_of(const CharT* s, size_type pos, size_type n) const noexcept {
        if (n <= m_len) {
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

    size_type find_last_not_of(const CharT* s, size_type pos = npos) const noexcept {
        return find_last_not_of(basic_str_view(s), pos);
    }

private:
    size_type m_len {0};
    const value_type* mp_str {nullptr};
};

template <typename CharT, typename Traits>
bool operator==(basic_str_view<CharT, Traits> lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    // Comparing the lengths first will omit unnecessary value comparison in compare().
    return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
}

template <typename CharT, typename Traits>
bool operator==(basic_str_view<CharT, Traits> lhs, const std::basic_string<CharT, Traits>& rhs) noexcept {
    return lhs == basic_str_view<CharT, Traits>(rhs);
}

template <typename CharT, typename Traits>
bool operator==(const std::basic_string<CharT, Traits>& lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    return basic_str_view<CharT, Traits>(lhs) == rhs;
}

template <typename CharT, typename Traits, std::size_t N>
bool operator==(basic_str_view<CharT, Traits> lhs, const CharT (&rhs)[N]) noexcept {
    // assume `rhs` is null terminated
    return lhs == basic_str_view<CharT, Traits>(rhs, N - 1);
}

template <typename CharT, typename Traits, std::size_t N>
bool operator==(const CharT (&lhs)[N], basic_str_view<CharT, Traits> rhs) noexcept {
    // assume `lhs` is null terminated
    return basic_str_view<CharT, Traits>(lhs, N - 1) == rhs;
}

template <typename CharT, typename Traits>
bool operator!=(basic_str_view<CharT, Traits> lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    return !(lhs == rhs);
}

template <typename CharT, typename Traits>
bool operator!=(basic_str_view<CharT, Traits> lhs, const std::basic_string<CharT, Traits>& rhs) noexcept {
    return !(lhs == basic_str_view<CharT, Traits>(rhs));
}

template <typename CharT, typename Traits, std::size_t N>
bool operator!=(basic_str_view<CharT, Traits> lhs, const CharT (&rhs)[N]) noexcept {
    // assume `rhs` is null terminated.
    return !(lhs == basic_str_view<CharT, Traits>(rhs, N - 1));
}

template <typename CharT, typename Traits, std::size_t N>
bool operator!=(const CharT (&lhs)[N], basic_str_view<CharT, Traits> rhs) noexcept {
    // assume `lhs` is null terminate
    return !(basic_str_view<CharT, Traits>(lhs, N - 1) == rhs);
}

template <typename CharT, typename Traits>
bool operator<(basic_str_view<CharT, Traits> lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    return lhs.compare(rhs) < 0;
}

template <typename CharT, typename Traits>
bool operator<=(basic_str_view<CharT, Traits> lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    return lhs.compare(rhs) <= 0;
}

template <typename CharT, typename Traits>
bool operator>(basic_str_view<CharT, Traits> lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    return lhs.compare(rhs) > 0;
}

template <typename CharT, typename Traits>
bool operator>=(basic_str_view<CharT, Traits> lhs, basic_str_view<CharT, Traits> rhs) noexcept {
    return lhs.compare(rhs) >= 0;
}

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, basic_str_view<CharT, Traits> sv) {
    return os.write(sv.data(), static_cast<std::streamsize>(sv.size()));
}

using str_view = basic_str_view<char>;

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_STR_VIEW_HPP_ */
