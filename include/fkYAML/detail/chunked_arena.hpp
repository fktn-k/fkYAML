//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.5.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_CHUNKED_ARENA_HPP
#define FK_YAML_DETAIL_CHUNKED_ARENA_HPP

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include <fkYAML/detail/macros/define_macros.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

template <typename T, std::size_t ChunkSize = 256>
class chunked_arena {
    using page_type = std::array<T, ChunkSize>;

public:
    using id_type = std::uint32_t;

    chunked_arena() = default;
    ~chunked_arena() = default;

    chunked_arena(const chunked_arena&) = delete;
    chunked_arena& operator=(const chunked_arena&) = delete;

    chunked_arena(chunked_arena&&) = default;
    chunked_arena& operator=(chunked_arena&&) = default;

    id_type allocate() {
        if (!m_free_ids.empty()) {
            id_type id = m_free_ids.back();
            m_free_ids.pop_back();
            return id;
        }

        const id_type id = m_next_id++;
        const id_type page_index = get_page_index(id);
        if (page_index >= m_pages.size()) {
            m_pages.emplace_back(new page_type());
        }
        return id;
    }

    void deallocate(const id_type id) {
        m_free_ids.push_back(id);
    }

    T& get(const id_type id) {
        const id_type page_index = get_page_index(id);
        const id_type offset = get_offset_in_page(id);
        return (*m_pages[page_index])[offset];
    }

    const T& get(const id_type id) const {
        const id_type page_index = get_page_index(id);
        const id_type offset = get_offset_in_page(id);
        return (*m_pages[page_index])[offset];
    }

    void clear() {
        // Clear the attributes of the chunked arena.
        // Note: The allocated pages are not deallocated for efficiency.
        m_free_ids.clear();
        m_next_id = 0;
    }

private:
    static id_type get_page_index(id_type id) noexcept {
        return id / ChunkSize;
    }

    static id_type get_offset_in_page(id_type id) noexcept {
        return id % ChunkSize;
    }

    std::vector<std::unique_ptr<page_type>> m_pages;
    std::vector<id_type> m_free_ids;
    id_type m_next_id {0};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif // FK_YAML_DETAIL_CHUNKED_ARENA_HPP
