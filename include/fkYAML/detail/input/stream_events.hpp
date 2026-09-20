//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.5.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FKYAML_DETAIL_INPUT_STREAM_EVENTS_HPP
#define FKYAML_DETAIL_INPUT_STREAM_EVENTS_HPP

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/input/block_scalar_header.hpp>
#include <fkYAML/detail/str_view.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

enum class collection_type {
    BLOCK,
    FLOW,
};

struct stream_start_event {};

struct stream_end_event {};

struct yaml_directive_event {
    yaml_directive_event(str_view version_)
        : version(version_) {
    }

    str_view version;
};

struct tag_directive_event {
    tag_directive_event(str_view handle_, str_view prefix_)
        : handle(handle_),
          prefix(prefix_) {
    }

    str_view handle;
    str_view prefix;
};

struct document_start_event {
    document_start_event(bool is_explicit_)
        : is_explicit(is_explicit_) {
    }

    bool is_explicit;
};

struct document_end_event {
    document_end_event(bool is_explicit_)
        : is_explicit(is_explicit_) {
    }

    bool is_explicit;
};

struct sequence_start_event {
    sequence_start_event(str_view tag_, str_view anchor_, collection_type type_)
        : tag(tag_),
          anchor(anchor_),
          type(type_) {
    }

    str_view tag;
    str_view anchor;
    collection_type type;
};

struct sequence_end_event {};

struct mapping_start_event {
    mapping_start_event(str_view tag_, str_view anchor_, collection_type type_)
        : tag(tag_),
          anchor(anchor_),
          type(type_) {
    }

    str_view tag;
    str_view anchor;
    collection_type type;
};

struct mapping_end_event {};

struct plain_scalar_event {
    plain_scalar_event(str_view value_, str_view tag_, str_view anchor_)
        : value(value_),
          tag(tag_),
          anchor(anchor_) {
    }

    str_view value;
    str_view tag;
    str_view anchor;
};

struct single_quoted_scalar_event {
    single_quoted_scalar_event(str_view value_, str_view tag_, str_view anchor_)
        : value(value_),
          tag(tag_),
          anchor(anchor_) {
    }

    str_view value;
    str_view tag;
    str_view anchor;
};

struct double_quoted_scalar_event {
    double_quoted_scalar_event(str_view value_, str_view tag_, str_view anchor_)
        : value(value_),
          tag(tag_),
          anchor(anchor_) {
    }

    str_view value;
    str_view tag;
    str_view anchor;
};

struct literal_scalar_event {
    literal_scalar_event(str_view value_, str_view tag_, str_view anchor_, block_scalar_header header_)
        : value(value_),
          tag(tag_),
          anchor(anchor_),
          header(header_) {
    }

    str_view value;
    str_view tag;
    str_view anchor;
    block_scalar_header header;
};

struct folded_scalar_event {
    folded_scalar_event(str_view value_, str_view tag_, str_view anchor_, block_scalar_header header_)
        : value(value_),
          tag(tag_),
          anchor(anchor_),
          header(header_) {
    }

    str_view value;
    str_view tag;
    str_view anchor;
    block_scalar_header header;
};

struct alias_event {
    alias_event(str_view value_)
        : value(value_) {
    }

    str_view value;
};

FK_YAML_DETAIL_NAMESPACE_END

#endif // FKYAML_DETAIL_INPUT_STREAM_EVENTS_HPP
