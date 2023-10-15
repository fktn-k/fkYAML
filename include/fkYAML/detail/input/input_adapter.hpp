/**
 *  _______   __ __   __  _____   __  __  __
 * |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
 * |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.1.1
 * |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
 *
 * SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
 * SPDX-License-Identifier: MIT
 *
 * @file
 */

#ifndef FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP_
#define FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP_

#include <cstdio>
#include <cstring>
#include <istream>
#include <iterator>
#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>

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

/**
 * @class iterator_input_adapter
 * @brief An input adapter for iterators.
 * @note This adapter requires at least bidirectional iterator tag.
 *
 * @tparam IterType An iterator type.
 */
template <typename IterType>
class iterator_input_adapter
{
public:
    /** A type for characters used in this input adapter. */
    using char_type = typename std::iterator_traits<IterType>::value_type;

    /**
     * @brief Construct a new iterator_input_adapter object.
     */
    iterator_input_adapter() = default;

    /**
     * @brief Construct a new iterator_input_adapter object.
     *
     * @param begin The beginning of iteraters.
     * @param end The end of iterators.
     */
    iterator_input_adapter(IterType begin, IterType end)
        : m_current(begin),
          m_begin(begin),
          m_end(end)
    {
    }

    /**
     * @brief Get a character at the current position and move forward.
     *
     * @return std::char_traits<char_type>::int_type A character or EOF.
     */
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

    /**
     * @brief Move backward if the current position is not at the beginning.
     */
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

/**
 * @class file_input_adapter
 * @brief An input adapter for C-style file handles.
 */
class file_input_adapter
{
public:
    /** A type for characters used in this input adapter. */
    using char_type = char;

    /**
     * @brief Construct a new file_input_adapter object.
     */
    file_input_adapter() = default;

    /**
     * @brief Construct a new file_input_adapter object
     *
     * @param file A file handle for this adapter. (A non-null pointer is assumed.)
     */
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

    /**
     * @brief Get a character at the current position and move forward.
     *
     * @return std::char_traits<char_type>::int_type A character or EOF.
     */
    typename std::char_traits<char_type>::int_type get_character()
    {
        return std::fgetc(m_file);
    }

    /**
     * @brief Move backward if the current position is not at the beginning.
     */
    void unget_character()
    {
        (void)std::fseek(m_file, -1, SEEK_CUR);
    }

private:
    std::FILE* m_file;
};

/**
 * @class stream_input_adapter
 * @brief An input adapter for streams
 */
class stream_input_adapter
{
public:
    /** A type for characters used in this input adapter. */
    using char_type = char;

    /**
     * @brief Construct a new stream_input_adapter object.
     */
    stream_input_adapter() = default;

    /**
     * @brief Construct a new stream_input_adapter object.
     *
     * @param is A reference to the target input stream.
     */
    explicit stream_input_adapter(std::istream& is)
        : m_istream(&is)
    {
    }

    // allow only move construct/assignment
    stream_input_adapter(const stream_input_adapter&) = delete;
    stream_input_adapter& operator=(const stream_input_adapter&) = delete;

    /**
     * @brief Construct a new stream_input_adapter object.
     *
     * @param rhs An adapter whose values are to be moved to this object.
     */
    stream_input_adapter(stream_input_adapter&& rhs) noexcept
        : m_istream(rhs.m_istream)
    {
        rhs.m_istream = nullptr;
    }

    /**
     * @brief A move assignment operator for stream_input_adapter.
     *
     * @param rhs An adapter whose values are to be moved to this object.
     * @return stream_input_adapter& Reference to this object.
     */
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

    /**
     * @brief Destroy the stream_input_adapter object.
     */
    ~stream_input_adapter() = default;

    /**
     * @brief Get a character at the current position and move forward.
     *
     * @return std::char_traits<char_type>::int_type A character or EOF.
     */
    std::char_traits<char_type>::int_type get_character()
    {
        return m_istream->get();
    }

    /**
     * @brief Move backward if the current position is not at the beginning.
     */
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

/**
 * @brief A factory method for iterator_input_adapter objects with ieterator values.
 *
 * @tparam ItrType An iterator type.
 * @param begin The beginning of iterators.
 * @param end The end of iterators.
 * @return iterator_input_adapter<ItrType> An iterator_input_adapter object for the target iterator type.
 */
template <typename ItrType>
inline iterator_input_adapter<ItrType> input_adapter(ItrType begin, ItrType end)
{
    return iterator_input_adapter<ItrType>(begin, end);
}

/**
 * @brief A factory method for iterator_input_adapter objects with char* objects.
 * @note This function assumes a null-terminated string as an argument.
 *
 * @tparam CharPtrType A pointer type for char.
 * @param ptr A pointer to the beginning of a target null-terminated string.
 * @return decltype(input_adapter(ptr, ptr + std::strlen(ptr)))
 */
template <
    typename CharPtrType, enable_if_t<
                              conjunction<
                                  std::is_pointer<CharPtrType>, negation<std::is_array<CharPtrType>>,
                                  std::is_integral<remove_pointer_t<CharPtrType>>,
                                  bool_constant<sizeof(remove_pointer_t<CharPtrType>) == 1>>::value,
                              int> = 0>
inline auto input_adapter(CharPtrType ptr) -> decltype(input_adapter(ptr, ptr + std::strlen(ptr)))
{
    return input_adapter(ptr, ptr + std::strlen(ptr));
}

/**
 * @brief A factory method for iterator_input_adapter objects with C-style arrays.
 *
 * @tparam T A type of arrayed objects.
 * @tparam N A size of an array.
 * @return decltype(input_adapter(array, array + N)) An iterator_input_adapter object for the target array.
 */
template <typename T, std::size_t N>
inline auto input_adapter(T (&array)[N]) -> decltype(input_adapter(array, array + N))
{
    return input_adapter(array, array + (N - 1));
}

/**
 * @brief A namespace to implement container_input_adapter_factory for internal use.
 */
namespace container_input_adapter_factory_impl
{

using std::begin;
using std::end;

/**
 * @brief A factory of input adapters for containers.
 *
 * @tparam ContainerType A container type.
 * @tparam typename N/A
 */
template <typename ContainerType, typename = void>
struct container_input_adapter_factory
{
};

/**
 * @brief A partial specialization of container_input_adapter_factory if begin()/end() are available for ContainerType.
 *
 * @tparam ContainerType A container type.
 */
template <typename ContainerType>
struct container_input_adapter_factory<
    ContainerType, void_t<decltype(begin(std::declval<ContainerType>()), end(std::declval<ContainerType>()))>>
{
    /** A type for resulting input adapter object. */
    using adapter_type =
        decltype(input_adapter(begin(std::declval<ContainerType>()), end(std::declval<ContainerType>())));

    /**
     * @brief A factory method of input adapter objects for the target container objects.
     *
     * @param container
     * @return adapter_type
     */
    static adapter_type create(const ContainerType& container)
    {
        return input_adapter(begin(container), end(container));
    }
};

} // namespace container_input_adapter_factory_impl

/**
 * @brief A factory method for iterator_input_adapter objects with containers.
 *
 * @tparam ContainerType A container type.
 * @param container A container object.
 * @return container_input_adapter_factory_impl::container_input_adapter_factory<ContainerType>::adapter_type
 */
template <typename ContainerType>
inline typename container_input_adapter_factory_impl::container_input_adapter_factory<ContainerType>::adapter_type
input_adapter(ContainerType&& container)
{
    return container_input_adapter_factory_impl::container_input_adapter_factory<ContainerType>::create(container);
}

/**
 * @brief A factory method for file_input_adapter objects with C-style file handles.
 *
 * @param file A file handle.
 * @return file_input_adapter A file_input_adapter object.
 */
inline file_input_adapter input_adapter(std::FILE* file)
{
    return file_input_adapter(file);
}

/**
 * @brief
 *
 * @param stream
 * @return stream_input_adapter
 */
inline stream_input_adapter input_adapter(std::istream& stream)
{
    return stream_input_adapter(stream);
}

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP_ */