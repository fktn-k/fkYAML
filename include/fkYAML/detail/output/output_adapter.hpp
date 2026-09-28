//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.5.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_OUTPUT_OUTPUT_ADAPTER_HPP
#define FK_YAML_DETAIL_OUTPUT_OUTPUT_ADAPTER_HPP

#include <cstdio>
#include <ostream>
#include <string>

#include <fkYAML/detail/macros/define_macros.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief A writer class that writes YAML content to a string.
class string_writer {
public:
    /// @brief Construct a new string_writer object for the given output string.
    /// @param output A reference to a string object representing the output destination.
    explicit string_writer(std::string& output) noexcept
        : mp_output(&output) {
    }

    /// @brief Write the given data to the output string.
    /// @param p_data Pointer to the data to be written.
    /// @param size The size of the data to be written.
    void write(const char* p_data, std::size_t size) {
        mp_output->append(p_data, size);
    }

private:
    /// @brief Reference to the output string representing the output destination.
    std::string* mp_output;
};

/// @brief A writer class that writes YAML content to a file.
class file_writer {
public:
    /// @brief Construct a new file_writer object for the given output file.
    /// @param p_file A pointer to a FILE object representing the output file.
    explicit file_writer(std::FILE* p_file) noexcept
        : mp_file(p_file) {
    }

    /// @brief Write the given data to the output file.
    /// @param p_data Pointer to the data to be written.
    /// @param size The size of the data to be written.
    void write(const char* p_data, std::size_t size) {
        std::fwrite(p_data, 1, size, mp_file); // NOLINT(cert-err33-c)
    }

private:
    /// @brief Pointer to the output file.
    std::FILE* mp_file;
};

/// @brief A writer class that writes YAML content to an output stream.
class ostream_writer {
public:
    /// @brief Construct a new ostream_writer object for the given output stream.
    /// @param os A reference to an output stream representing the output destination.
    explicit ostream_writer(std::ostream& os) noexcept
        : mp_os(&os) {
    }

    /// @brief Write the given data to the output stream.
    /// @param p_data Pointer to the data to be written.
    /// @param size The size of the data to be written.
    void write(const char* p_data, std::size_t size) {
        mp_os->write(p_data, static_cast<std::streamsize>(size));
    }

private:
    /// @brief Pointer to the output stream representing the output destination.
    std::ostream* mp_os;
};

/// @brief An adapter class to unify different output targets for writing YAML content.
class output_adapter {
public:
    /// @brief Construct a new output_adapter object for the given writer.
    /// @tparam Writer The type of the underlying writer. It must provide a write(const char*, std::size_t) method.
    /// @param writer The underlying writer object to which YAML content will be written.
    template <typename Writer>
    explicit output_adapter(Writer& writer)
        : mp_writer(static_cast<void*>(&writer)),
          m_write([](void* p_writer, const char* p_data, std::size_t size) {
              static_cast<Writer*>(p_writer)->write(p_data, size);
          }) {
    }

    /// @brief Write the given data to the underlying writer.
    /// @param p_data Pointer to the data to be written.
    /// @param size The size of the data to be written.
    void write(const char* p_data, std::size_t size) {
        m_write(mp_writer, p_data, size);
    }

private:
    /// @brief Pointer to the underlying writer object.
    void* mp_writer;
    /// @brief Function pointer to the write method of the underlying writer.
    void (*m_write)(void*, const char*, std::size_t);
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_OUTPUT_OUTPUT_ADAPTER_HPP */
