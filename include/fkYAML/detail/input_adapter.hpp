/**
 *  _______   __ __   __  _____   __  __  __
 * |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
 * |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
 * |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
 *
 * SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
 * SPDX-License-Identifier: MIT
 *
 * @file
 */

#ifndef FK_YAML_DETAIL_INPUT_ADAPTER_HPP_
#define FK_YAML_DETAIL_INPUT_ADAPTER_HPP_

#include <cstdio>
#include <cstring>
#include <istream>
#include <iterator>
#include <string>

#include "fkYAML/detail/version_macros.hpp"
#include "fkYAML/detail/stl_supplement.hpp"

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/**
 * @namespace detail
 * @brief namespace for internal implementations of fkYAML library.
 */
namespace detail
{

///////////////////////
//   input_adapter   //
///////////////////////

// TODO: this adapter requires at least bidirectional iterator tag.
template <typename IterType>
class iterator_input_adapter
{
public:
    using char_type = typename std::iterator_traits<IterType>::value_type;

    iterator_input_adapter() = default;

    iterator_input_adapter(IterType begin, IterType end)
        : m_current(begin),
          m_begin(begin),
          m_end(end)
    {
    }

    typename std::char_traits<char_type>::int_type get_character()
    {
        if (m_current != m_end)
        {
            auto ret = std::char_traits<char_type>::to_int_type(*m_current);
            ++m_current;
            return ret;
        }

        return std::char_traits<char_type>::eof();
    }

    void unget_character()
    {
        if (m_current != m_begin)
        {
            --m_current;
        }
    }

private:
    IterType m_current;
    IterType m_begin;
    IterType m_end;
};

class file_input_adapter
{
public:
    using char_type = char;

    file_input_adapter() = default;

    explicit file_input_adapter(std::FILE* file)
        : m_file(file)
    {
    }

    // allow only move construct/assignment
    file_input_adapter(const file_input_adapter&) = delete;
    file_input_adapter(file_input_adapter&& rhs) = default;
    file_input_adapter& operator=(const file_input_adapter&) = delete;
    file_input_adapter& operator=(file_input_adapter&&) = default;
    ~file_input_adapter() = default;

    typename std::char_traits<char_type>::int_type get_character()
    {
        return std::fgetc(m_file);
    }

    void unget_character()
    {
        (void)std::fseek(m_file, -1, SEEK_CUR);
    }

private:
    std::FILE* m_file;
};

class stream_input_adapter
{
public:
    using char_type = char;

    stream_input_adapter() = default;

    explicit stream_input_adapter(std::istream& is)
        : m_istream(&is)
    {
    }

    // allow only move construct/assignment
    stream_input_adapter(const stream_input_adapter&) = delete;
    stream_input_adapter& operator=(const stream_input_adapter&) = delete;

    stream_input_adapter(stream_input_adapter&& rhs) noexcept
        : m_istream(rhs.m_istream)
    {
        rhs.m_istream = nullptr;
    }

    stream_input_adapter& operator=(stream_input_adapter&& rhs) noexcept
    {
        if (&rhs == this)
        {
            return *this;
        }

        m_istream = rhs.m_istream;

        rhs.m_istream = nullptr;

        return *this;
    }

    ~stream_input_adapter() = default;

    std::char_traits<char_type>::int_type get_character()
    {
        return m_istream->get();
    }

    void unget_character()
    {
        m_istream->unget();
    }

private:
    std::istream* m_istream;
};

///////////////////////////////////
//   iterator_adapter provider   //
///////////////////////////////////

template <typename ItrType>
iterator_input_adapter<ItrType> input_adapter(ItrType begin, ItrType end)
{
    return iterator_input_adapter<ItrType>(begin, end);
}

template <
    typename CharPtrType, enable_if_t<
                              conjunction<
                                  std::is_pointer<CharPtrType>, negation<std::is_array<CharPtrType>>,
                                  std::is_integral<remove_pointer_t<CharPtrType>>,
                                  bool_constant<sizeof(remove_pointer_t<CharPtrType>) == 1>>::value,
                              int> = 0>
auto input_adapter(CharPtrType ptr) -> decltype(input_adapter(ptr, ptr + std::strlen(ptr)))
{
    return input_adapter(ptr, ptr + std::strlen(ptr));
}

template <typename T, std::size_t N>
auto input_adapter(T (&array)[N]) -> decltype(input_adapter(array, array + N))
{
    return input_adapter(array, array + (N - 1));
}

namespace container_input_adapter_factory_impl
{

using std::begin;
using std::end;

template <typename ContainerType, typename = void>
struct container_input_adapter_factory
{
};

template <typename ContainerType>
struct container_input_adapter_factory<
    ContainerType, void_t<decltype(begin(std::declval<ContainerType>()), end(std::declval<ContainerType>()))>>
{
    using adapter_type =
        decltype(input_adapter(begin(std::declval<ContainerType>()), end(std::declval<ContainerType>())));

    static adapter_type create(const ContainerType& container)
    {
        return input_adapter(begin(container), end(container));
    }
};

} // namespace container_input_adapter_factory_impl

template <typename ContainerType>
typename container_input_adapter_factory_impl::container_input_adapter_factory<ContainerType>::adapter_type
input_adapter(ContainerType&& container)
{
    return container_input_adapter_factory_impl::container_input_adapter_factory<ContainerType>::create(container);
}

inline file_input_adapter input_adapter(std::FILE* file)
{
    return file_input_adapter(file);
}

inline stream_input_adapter input_adapter(std::istream& stream)
{
    return stream_input_adapter(stream);
}

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_ADAPTER_HPP_ */