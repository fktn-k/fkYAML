//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATION_COMPOSITOR_HPP
#define FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATION_COMPOSITOR_HPP

#include <cstddef>
#include <deque>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <fkYAML/detail/conversions/scalar_conv.hpp>
#include <fkYAML/detail/input/scalar_scanner.hpp>
#include <fkYAML/node_type.hpp>

#include "event.hpp"
#include "validation_error.hpp"
#include "validator.hpp"

namespace yaml_test_suite_runner {

enum class validation_mode {
    YAML,
    JSON,
};

namespace detail {

inline const std::string* find_event_param(const std::vector<event_param>& params, event_param_type type) {
    for (const auto& param : params) {
        if (param.first == type) {
            return &param.second;
        }
    }
    return nullptr;
}

inline bool try_convert_null(const std::string& value) {
    std::nullptr_t null = nullptr;
    return fkyaml::detail::aton(value.c_str(), value.c_str() + value.size(), null);
}

template <typename T>
bool try_convert_bool(const std::string& value, T& converted_value) {
    return fkyaml::detail::atob(value.c_str(), value.c_str() + value.size(), converted_value);
}

template <typename T>
bool try_convert_integer(const std::string& value, T& converted_value) {
    return fkyaml::detail::atoi(value.c_str(), value.c_str() + value.size(), converted_value);
}

template <typename T>
bool try_convert_float(const std::string& value, T& converted_value) {
    return fkyaml::detail::atof(value.c_str(), value.c_str() + value.size(), converted_value);
}

} // namespace detail

template <typename T>
class validation_compositor {
public:
    explicit validation_compositor(validation_mode mode = validation_mode::YAML)
        : m_mode(mode) {
    }

    ~validation_compositor() = default;

    void handle_event(event&& evt) {
        switch (evt.type) {
        case event_type::STREAM_START:
            handle_stream_start();
            break;
        case event_type::STREAM_END:
            handle_stream_end();
            break;
        case event_type::DOCUMENT_START:
            handle_document_start();
            break;
        case event_type::DOCUMENT_END:
            handle_document_end();
            break;
        case event_type::SEQUENCE_START:
            handle_container_start(frame_kind::SEQUENCE, std::move(evt.params));
            break;
        case event_type::SEQUENCE_END:
            handle_container_end(frame_kind::SEQUENCE);
            break;
        case event_type::MAPPING_START:
            handle_container_start(frame_kind::MAPPING, std::move(evt.params));
            break;
        case event_type::MAPPING_END:
            handle_container_end(frame_kind::MAPPING);
            break;
        case event_type::SCALAR:
            append_completed_node(make_scalar_node(std::move(evt.params)));
            break;
        case event_type::ALIAS:
            append_completed_node(make_alias_node(std::move(evt.params)));
            break;
        }
    }

    std::vector<std::unique_ptr<validator<T>>> take_document_validators() {
        ensure_composition_completed();
        return std::move(m_document_validators);
    }

    std::unique_ptr<validator<T>> take_validator() {
        ensure_composition_completed();
        if (m_document_validators.size() != 1) {
            throw std::runtime_error(
                "Expected exactly one document validator, got " + std::to_string(m_document_validators.size()) + ".");
        }

        std::unique_ptr<validator<T>> validator_ptr = std::move(m_document_validators.front());
        m_document_validators.clear();
        return validator_ptr;
    }

private:
    enum class expected_node_kind {
        SCALAR,
        SEQUENCE,
        MAPPING,
        ALIAS,
    };

    struct expected_mapping_entry;

    struct expected_node {
        expected_node_kind kind {expected_node_kind::SCALAR};
        std::string anchor_name {};
        std::string tag_name {};
        std::string scalar_value {};
        std::vector<expected_node> sequence_items {};
        std::vector<expected_mapping_entry> mapping_entries {};

        expected_node() = default;
        expected_node(const expected_node&) = delete;
        expected_node& operator=(const expected_node&) = delete;
        expected_node(expected_node&&) noexcept = default;
        expected_node& operator=(expected_node&&) noexcept = default;
    };

    struct expected_mapping_entry {
        std::unique_ptr<expected_node> key;
        std::unique_ptr<expected_node> value;

        expected_mapping_entry() = default;
        expected_mapping_entry(const expected_mapping_entry&) = delete;
        expected_mapping_entry& operator=(const expected_mapping_entry&) = delete;
        expected_mapping_entry(expected_mapping_entry&&) noexcept = default;
        expected_mapping_entry& operator=(expected_mapping_entry&&) noexcept = default;
    };

    enum class frame_kind {
        DOCUMENT,
        SEQUENCE,
        MAPPING,
    };

    struct frame {
        explicit frame(frame_kind frame_type)
            : kind(frame_type) {
        }

        frame(const frame&) = delete;
        frame& operator=(const frame&) = delete;
        frame(frame&&) noexcept = default;
        frame& operator=(frame&&) noexcept = default;

        frame_kind kind;
        std::string anchor_name {};
        std::string tag_name {};
        std::unique_ptr<expected_node> root;
        std::vector<expected_node> sequence_items {};
        std::vector<expected_mapping_entry> mapping_entries {};
        std::unique_ptr<expected_node> pending_key;
    };

    struct scalar_expectation {
        fkyaml::node_type type {fkyaml::node_type::STRING};
        std::string value {};
    };

    void handle_stream_start() {
        if (m_stream_started) {
            throw std::runtime_error("Unexpected STREAM_START event.");
        }
        if (!m_frames.empty() || !m_document_validators.empty()) {
            throw std::runtime_error("STREAM_START must be the first event in the stream.");
        }

        m_stream_started = true;
        m_stream_ended = false;
    }

    void handle_stream_end() {
        if (!m_stream_started || m_stream_ended) {
            throw std::runtime_error("Unexpected STREAM_END event.");
        }
        if (!m_frames.empty()) {
            throw std::runtime_error("Cannot finish stream while a document or container is still open.");
        }

        m_stream_ended = true;
    }

    void handle_document_start() {
        if (!m_stream_started || m_stream_ended) {
            throw std::runtime_error("DOCUMENT_START must occur inside an open stream.");
        }
        if (!m_frames.empty()) {
            throw std::runtime_error("Nested YAML documents are not supported.");
        }

        m_frames.emplace_back(frame_kind::DOCUMENT);
    }

    void handle_document_end() {
        if (m_frames.empty() || m_frames.back().kind != frame_kind::DOCUMENT) {
            throw std::runtime_error("Unexpected DOCUMENT_END event.");
        }
        if (!m_frames.back().root) {
            throw std::runtime_error("Document ended without a root node.");
        }

        m_document_validators.emplace_back(compile_validator(*m_frames.back().root));
        m_frames.pop_back();
    }

    void handle_container_start(frame_kind kind, std::vector<event_param>&& params) {
        if (m_frames.empty()) {
            throw std::runtime_error("Container start event must occur inside a document.");
        }

        frame next_frame(kind);
        if (const std::string* anchor_name = detail::find_event_param(params, event_param_type::ANCHOR)) {
            next_frame.anchor_name = *anchor_name;
        }
        if (const std::string* tag_name = detail::find_event_param(params, event_param_type::TAG)) {
            next_frame.tag_name = *tag_name;
        }

        m_frames.emplace_back(std::move(next_frame));
    }

    void handle_container_end(frame_kind kind) {
        if (m_frames.empty() || m_frames.back().kind != kind) {
            throw std::runtime_error("Unexpected container end event.");
        }

        expected_node node;
        node.kind = (kind == frame_kind::SEQUENCE) ? expected_node_kind::SEQUENCE : expected_node_kind::MAPPING;
        node.anchor_name = std::move(m_frames.back().anchor_name);
        node.tag_name = std::move(m_frames.back().tag_name);

        if (kind == frame_kind::SEQUENCE) {
            node.sequence_items = std::move(m_frames.back().sequence_items);
        }
        else {
            if (m_frames.back().pending_key) {
                throw std::runtime_error("Mapping ended while waiting for a value node.");
            }
            node.mapping_entries = std::move(m_frames.back().mapping_entries);
        }

        m_frames.pop_back();
        append_completed_node(std::move(node));
    }

    expected_node make_scalar_node(std::vector<event_param>&& params) {
        expected_node node;
        node.kind = expected_node_kind::SCALAR;

        if (const std::string* anchor_name = detail::find_event_param(params, event_param_type::ANCHOR)) {
            node.anchor_name = *anchor_name;
        }
        if (const std::string* tag_name = detail::find_event_param(params, event_param_type::TAG)) {
            node.tag_name = *tag_name;
        }
        if (const std::string* scalar_value = detail::find_event_param(params, event_param_type::VALUE)) {
            node.scalar_value = *scalar_value;
        }
        else {
            throw std::runtime_error("Scalar event is missing a value parameter.");
        }

        return node;
    }

    expected_node make_alias_node(std::vector<event_param>&& params) {
        expected_node node;
        node.kind = expected_node_kind::ALIAS;

        if (const std::string* alias_name = detail::find_event_param(params, event_param_type::ALIAS)) {
            node.anchor_name = *alias_name;
        }
        else {
            throw std::runtime_error("Alias event is missing an alias parameter.");
        }

        return node;
    }

    void append_completed_node(expected_node&& node) {
        if (m_frames.empty()) {
            throw std::runtime_error("Value event must occur inside a document.");
        }

        frame& target = m_frames.back();
        switch (target.kind) {
        case frame_kind::DOCUMENT:
            if (target.root) {
                throw std::runtime_error("Document already has a root node.");
            }
            target.root.reset(new expected_node(std::move(node)));
            break;
        case frame_kind::SEQUENCE:
            target.sequence_items.emplace_back(std::move(node));
            break;
        case frame_kind::MAPPING:
            if (!target.pending_key) {
                target.pending_key.reset(new expected_node(std::move(node)));
            }
            else {
                expected_mapping_entry entry;
                entry.key = std::move(target.pending_key);
                entry.value.reset(new expected_node(std::move(node)));
                target.mapping_entries.emplace_back(std::move(entry));
            }
            break;
        }
    }

    void ensure_composition_completed() const {
        if (!m_stream_started || !m_stream_ended) {
            throw std::runtime_error("Cannot take validators before the event stream has finished.");
        }
        if (!m_frames.empty()) {
            throw std::runtime_error("Cannot take validators while a document or container is still open.");
        }
    }

    std::unique_ptr<validator<T>> compile_validator(const expected_node& node) const {
        std::map<std::string, const expected_node*> anchor_registry;
        return compile_validator(node, anchor_registry);
    }

    std::unique_ptr<validator<T>> compile_validator(
        const expected_node& node, std::map<std::string, const expected_node*>& anchor_registry) const {
        if (node.kind != expected_node_kind::ALIAS && !node.anchor_name.empty()) {
            anchor_registry[node.anchor_name] = &node;
        }

        if (m_mode == validation_mode::JSON && node.kind == expected_node_kind::ALIAS) {
            typename std::map<std::string, const expected_node*>::const_iterator anchor_itr =
                anchor_registry.find(node.anchor_name);
            if (anchor_itr == anchor_registry.end()) {
                throw validation_error("Alias validation failed: unknown anchor name " + node.anchor_name);
            }

            return compile_validator(*anchor_itr->second, anchor_registry);
        }

        std::vector<std::unique_ptr<validator<T>>> validators;

        switch (node.kind) {
        case expected_node_kind::SEQUENCE:
            validators.emplace_back(new type_validator<T>(fkyaml::node_type::SEQUENCE));
            validators.emplace_back(new size_validator<T>(node.sequence_items.size()));
            for (std::size_t index = 0; index < node.sequence_items.size(); ++index) {
                validators.emplace_back(new sequence_item_validator<T>(
                    index, compile_validator(node.sequence_items[index], anchor_registry)));
            }
            break;
        case expected_node_kind::MAPPING:
            validators.emplace_back(new type_validator<T>(fkyaml::node_type::MAPPING));
            validators.emplace_back(new size_validator<T>(node.mapping_entries.size()));
            for (const auto& entry : node.mapping_entries) {
                validators.emplace_back(new mapping_entry_validator<T>(
                    compile_validator(*entry.key, anchor_registry), compile_validator(*entry.value, anchor_registry)));
            }
            break;
        case expected_node_kind::ALIAS:
            validators.emplace_back(
                new alias_validator<T>(std::unique_ptr<validator<T>>(new anchor_name_validator<T>(node.anchor_name))));
            break;
        case expected_node_kind::SCALAR: {
            const scalar_expectation scalar = make_scalar_expectation(node);
            validators.emplace_back(new type_validator<T>(scalar.type));
            append_scalar_value_validator(scalar, validators);
            break;
        }
        }

        if (m_mode == validation_mode::YAML && !node.tag_name.empty()) {
            validators.emplace_back(new tag_validator<T>(node.tag_name));
        }
        if (m_mode == validation_mode::YAML && !node.anchor_name.empty() && node.kind != expected_node_kind::ALIAS) {
            validators.emplace_back(
                new anchor_validator<T>(std::unique_ptr<validator<T>>(new anchor_name_validator<T>(node.anchor_name))));
        }

        return std::unique_ptr<validator<T>>(new composite_validator<T>(std::move(validators)));
    }

    scalar_expectation make_scalar_expectation(const expected_node& node) const {
        if (node.scalar_value.empty()) {
            throw std::runtime_error("Scalar event must include a style marker.");
        }

        scalar_expectation scalar;
        scalar.value = node.scalar_value.substr(1);

        if (node.tag_name == "tag:yaml.org,2002:null") {
            scalar.type = fkyaml::node_type::NULL_OBJECT;
            return scalar;
        }
        if (node.tag_name == "tag:yaml.org,2002:bool") {
            scalar.type = fkyaml::node_type::BOOLEAN;
            return scalar;
        }
        if (node.tag_name == "tag:yaml.org,2002:int") {
            scalar.type = fkyaml::node_type::INTEGER;
            return scalar;
        }
        if (node.tag_name == "tag:yaml.org,2002:float") {
            scalar.type = fkyaml::node_type::FLOAT;
            return scalar;
        }
        if (node.tag_name == "tag:yaml.org,2002:str") {
            scalar.type = fkyaml::node_type::STRING;
            return scalar;
        }

        const char style = node.scalar_value[0];
        if (style != ':') {
            scalar.type = fkyaml::node_type::STRING;
            return scalar;
        }

        scalar.type =
            fkyaml::detail::scalar_scanner::scan(scalar.value.c_str(), scalar.value.c_str() + scalar.value.size());
        return scalar;
    }

    void append_scalar_value_validator(
        const scalar_expectation& scalar, std::vector<std::unique_ptr<validator<T>>>& validators) const {
        switch (scalar.type) {
        case fkyaml::node_type::NULL_OBJECT:
            if (!detail::try_convert_null(scalar.value)) {
                throw validation_error("Failed to interpret expected scalar as null: " + scalar.value);
            }
            break;
        case fkyaml::node_type::BOOLEAN: {
            typename T::boolean_type converted_value = static_cast<typename T::boolean_type>(false);
            if (!detail::try_convert_bool(scalar.value, converted_value)) {
                throw validation_error("Failed to interpret expected scalar as boolean: " + scalar.value);
            }
            validators.emplace_back(new value_validator<T, typename T::boolean_type>(converted_value));
            break;
        }
        case fkyaml::node_type::INTEGER: {
            typename T::integer_type converted_value = 0;
            if (!detail::try_convert_integer(scalar.value, converted_value)) {
                throw validation_error("Failed to interpret expected scalar as integer: " + scalar.value);
            }
            validators.emplace_back(new value_validator<T, typename T::integer_type>(converted_value));
            break;
        }
        case fkyaml::node_type::FLOAT: {
            typename T::float_number_type converted_value = static_cast<typename T::float_number_type>(0.0);
            if (!detail::try_convert_float(scalar.value, converted_value)) {
                throw validation_error("Failed to interpret expected scalar as float: " + scalar.value);
            }
            validators.emplace_back(new value_validator<T, typename T::float_number_type>(converted_value));
            break;
        }
        case fkyaml::node_type::STRING:
            validators.emplace_back(new value_validator<T, typename T::string_type>(scalar.value));
            break;
        case fkyaml::node_type::SEQUENCE:
        case fkyaml::node_type::MAPPING:
            throw validation_error("Scalar event resolved to a non-scalar node type.");
        }
    }

    bool m_stream_started {false};
    bool m_stream_ended {false};
    std::deque<frame> m_frames {};
    std::vector<std::unique_ptr<validator<T>>> m_document_validators {};
    validation_mode m_mode {validation_mode::YAML};
};

} // namespace yaml_test_suite_runner

#endif // FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATION_COMPOSITOR_HPP
