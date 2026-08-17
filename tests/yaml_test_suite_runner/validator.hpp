//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATOR_HPP
#define FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATOR_HPP

#include "validation_error.hpp"

namespace yaml_test_suite_runner {

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
    type_validator(typename T::node_type expected_type)
        : m_expected_type(expected_type) {
    }

    ~type_validator() override = default;

    void validate(const T& value) const override {
        if (value.get_node_type() != m_expected_type) {
            throw validation_error(
                "Type validation failed: expected " + std::to_string(m_expected_type) + ", got " +
                std::to_string(value.get_node_type()));
        }
    }

private:
    typename T::node_type m_expected_type;
};

template <typename T, typename ValueType>
class value_validator : public validator<T> {
public:
    value_validator(const ValueType& expected_value)
        : m_expected_value(expected_value) {
    }

    ~value_validator() override = default;

    void validate(const T& value) const override {
        if (value.get_value<ValueType>() != m_expected_value) {
            throw validation_error(
                "Value validation failed: expected " + std::to_string(m_expected_value) + ", got " +
                std::to_string(value.get_value<ValueType>()));
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
        if (value.get_tag_name() != m_expected_tag) {
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

} // namespace yaml_test_suite_runner

#endif // FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATOR_HPP
