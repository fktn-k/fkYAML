//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATOR_HPP
#define FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATOR_HPP

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <fkYAML/node_type.hpp>

#include "validation_error.hpp"

namespace yaml_test_suite_runner {

namespace detail {

template <typename ValueType>
std::string debug_string(const ValueType& value) {
    std::ostringstream oss;
    oss << std::boolalpha << value;
    return oss.str();
}

inline bool is_verbatim_tag(const std::string& tag) {
    return tag.size() >= 4 && tag[0] == '!' && tag[1] == '<' && tag[tag.size() - 1] == '>';
}

inline bool tags_equivalent(const std::string& expected_tag, const std::string& actual_tag) {
    if (expected_tag == actual_tag) {
        return true;
    }

    const std::size_t actual_handle_pos = actual_tag.find('!', 1);
    if (actual_handle_pos != std::string::npos && actual_handle_pos + 1 < actual_tag.size()) {
        const std::string actual_suffix = actual_tag.substr(actual_handle_pos + 1);

        if (is_verbatim_tag(expected_tag)) {
            const std::string expanded = expected_tag.substr(2, expected_tag.size() - 3);
            return expanded.size() >= actual_suffix.size() &&
                   expanded.compare(expanded.size() - actual_suffix.size(), actual_suffix.size(), actual_suffix) == 0;
        }

        return expected_tag.size() >= actual_suffix.size() &&
               expected_tag.compare(expected_tag.size() - actual_suffix.size(), actual_suffix.size(), actual_suffix) ==
                   0;
    }

    return false;
}

} // namespace detail

template <typename T>
class validator {
public:
    virtual ~validator() = default;

    virtual void validate(const T& value) const = 0;
};

template <typename T>
class composite_validator : public validator<T> {
public:
    composite_validator(std::vector<std::unique_ptr<validator<T>>>&& validators)
        : m_validators(std::move(validators)) {
    }

    ~composite_validator() override = default;

    void validate(const T& value) const override {
        for (const auto& validator : m_validators) {
            validator->validate(value);
        }
    }

private:
    std::vector<std::unique_ptr<validator<T>>> m_validators;
};

template <typename T>
class size_validator : public validator<T> {
public:
    size_validator(std::size_t expected_size)
        : m_expected_size(expected_size) {
    }

    ~size_validator() override = default;

    void validate(const T& value) const override {
        if (value.size() != m_expected_size) {
            throw validation_error(
                "Size validation failed: expected " + std::to_string(m_expected_size) + ", got " +
                std::to_string(value.size()));
        }
    }

private:
    std::size_t m_expected_size {0};
};

template <typename T>
class type_validator : public validator<T> {
public:
    type_validator(fkyaml::node_type expected_type)
        : m_expected_type(expected_type) {
    }

    ~type_validator() override = default;

    void validate(const T& value) const override {
        if (value.get_type() != m_expected_type) {
            throw validation_error(
                "Type validation failed: expected " + std::to_string(static_cast<int>(m_expected_type)) + ", got " +
                std::to_string(static_cast<int>(value.get_type())));
        }
    }

private:
    fkyaml::node_type m_expected_type;
};

template <typename T, typename ValueType>
class value_validator : public validator<T> {
public:
    value_validator(const ValueType& expected_value)
        : m_expected_value(expected_value) {
    }

    ~value_validator() override = default;

    void validate(const T& value) const override {
        if (value.template get_value<ValueType>() != m_expected_value) {
            throw validation_error(
                "Value validation failed: expected " + detail::debug_string(m_expected_value) + ", got " +
                detail::debug_string(value.template get_value<ValueType>()));
        }
    }

private:
    ValueType m_expected_value;
};

template <typename T>
class tag_validator : public validator<T> {
public:
    tag_validator(const std::string& expected_tag)
        : m_expected_tag(expected_tag) {
    }

    ~tag_validator() override = default;

    void validate(const T& value) const override {
        if (!value.has_tag_name()) {
            throw validation_error("Tag validation failed: expected " + m_expected_tag + ", but no tag found");
        }
        if (!detail::tags_equivalent(m_expected_tag, value.get_tag_name())) {
            throw validation_error(
                "Tag validation failed: expected " + m_expected_tag + ", got " + value.get_tag_name());
        }
    }

private:
    std::string m_expected_tag;
};

template <typename T>
class anchor_name_validator : public validator<T> {
public:
    anchor_name_validator(const std::string& expected_anchor_name)
        : m_expected_anchor_name(expected_anchor_name) {
    }

    ~anchor_name_validator() override = default;

    void validate(const T& value) const override {
        if (!value.has_anchor_name()) {
            throw validation_error(
                "Anchor validation failed: expected " + m_expected_anchor_name + ", but no anchor found");
        }
        if (value.get_anchor_name() != m_expected_anchor_name) {
            throw validation_error(
                "Anchor validation failed: expected " + m_expected_anchor_name + ", got " + value.get_anchor_name());
        }
    }

private:
    std::string m_expected_anchor_name;
};

template <typename T>
class anchor_validator : public validator<T> {
public:
    anchor_validator(std::unique_ptr<validator<T>> anchor_name_validator)
        : m_anchor_name_validator(std::move(anchor_name_validator)) {
    }

    ~anchor_validator() override = default;

    void validate(const T& value) const override {
        if (!value.is_anchor()) {
            throw validation_error("Anchor validation failed: expected an anchor node, but is not");
        }
        m_anchor_name_validator->validate(value);
    }

private:
    std::unique_ptr<validator<T>> m_anchor_name_validator;
};

template <typename T>
class alias_validator : public validator<T> {
public:
    alias_validator(std::unique_ptr<validator<T>> anchor_name_validator)
        : m_anchor_name_validator(std::move(anchor_name_validator)) {
    }

    ~alias_validator() override = default;

    void validate(const T& value) const override {
        if (!value.is_alias()) {
            throw validation_error("Alias validation failed: expected an alias node, but is not");
        }
        m_anchor_name_validator->validate(value);
    }

private:
    std::unique_ptr<validator<T>> m_anchor_name_validator;
};

template <typename T>
class sequence_item_validator : public validator<T> {
public:
    sequence_item_validator(std::size_t index, std::unique_ptr<validator<T>> child_validator)
        : m_index(index),
          m_child_validator(std::move(child_validator)) {
    }

    ~sequence_item_validator() override = default;

    void validate(const T& value) const override {
        if (!value.is_sequence()) {
            throw validation_error("Sequence item validation failed: target node is not a sequence.");
        }
        if (value.size() <= m_index) {
            throw validation_error(
                "Sequence item validation failed: expected index " + std::to_string(m_index) + ", got size " +
                std::to_string(value.size()));
        }

        m_child_validator->validate(value.at(m_index));
    }

private:
    std::size_t m_index {0};
    std::unique_ptr<validator<T>> m_child_validator;
};

template <typename T>
class mapping_entry_validator : public validator<T> {
public:
    mapping_entry_validator(std::unique_ptr<validator<T>> key_validator, std::unique_ptr<validator<T>> value_validator)
        : m_key_validator(std::move(key_validator)),
          m_value_validator(std::move(value_validator)) {
    }

    ~mapping_entry_validator() override = default;

    void validate(const T& value) const override {
        if (!value.is_mapping()) {
            throw validation_error("Mapping entry validation failed: target node is not a mapping.");
        }

        bool key_matched = false;
        std::string value_error_message;
        for (auto& entry : value.map_items()) {
            try {
                m_key_validator->validate(entry.key());
            }
            catch (const validation_error&) {
                continue;
            }

            key_matched = true;
            try {
                m_value_validator->validate(entry.value());
                return;
            }
            catch (const validation_error& ex) {
                value_error_message = ex.what();
            }
        }

        if (key_matched) {
            throw validation_error("Mapping entry validation failed: " + value_error_message);
        }

        throw validation_error("Mapping entry validation failed: no matching key was found.");
    }

private:
    std::unique_ptr<validator<T>> m_key_validator;
    std::unique_ptr<validator<T>> m_value_validator;
};

} // namespace yaml_test_suite_runner

#endif // FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATOR_HPP
