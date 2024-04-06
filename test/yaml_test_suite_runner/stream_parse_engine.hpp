//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.13
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_YAML_TEST_SUITE_RUNNER_EVENT_STREAM_ENGINE_HPP
#define FK_YAML_YAML_TEST_SUITE_RUNNER_EVENT_STREAM_ENGINE_HPP

#include <cassert>
#include <string>

#include <fkYAML/node.hpp>

#include "stream_event_callback.hpp"
#include "stream_event_source.hpp"

class stream_parse_engine {
public:
    using str_view = fkyaml::detail::str_view;

    stream_parse_engine(stream_event_callback& callback) noexcept
        : mp_callback(&callback) {
    }

    ~stream_parse_engine() noexcept = default;

    void parse(stream_event_source& source) {
        assert(mp_callback != nullptr);
        stream_event_callback& callback = *mp_callback;

        for (str_view line = source.get_line(); !line.empty(); line = source.get_line()) {
            if (line.starts_with("+STR")) {
                callback.on_stream_begin();
            }
            else if (line.starts_with("-STR")) {
                callback.on_stream_end();
            }
            else if (line.starts_with("+DOC")) {
                callback.on_document_begin();
            }
            else if (line.starts_with("-DOC")) {
                callback.on_document_end();
            }
            else if (line.starts_with("+SEQ")) {
                str_view anchor {};
                str_view tag {};

                if (line.size() > 4) {
                    line.remove_prefix(5); // skip "+SEQ ".
                    parse_container_begin_attrs(line, "[]", anchor, tag);
                }

                callback.on_seq_begin(anchor, tag);
            }
            else if (line.starts_with("-SEQ")) {
                callback.on_seq_end();
            }
            else if (line.starts_with("+MAP")) {
                str_view anchor {};
                str_view tag {};

                if (line.size() > 4) {
                    line.remove_prefix(5); // skip "+MAP ".
                    parse_container_begin_attrs(line, "{}", anchor, tag);
                }

                callback.on_map_begin(anchor, tag);
            }
            else if (line.starts_with("-MAP")) {
                callback.on_map_end();
            }
            else if (line.starts_with("=VAL")) {
                assert(line.size() > 5);
                assert(line[4] == ' ');
                line.remove_prefix(5); // skip "=VAL ".

                str_view anchor {};
                str_view tag {};
                if (line.starts_with('&') || line.starts_with('<')) {
                    parse_anchor_and_tag(line, anchor, tag);
                }

                assert(!line.empty());
                assert(str_view(":\'\"|>").contains(line[0]));
                line.remove_prefix(1);

                std::size_t pos = 0;
                pos = line.find_first_of('\\');
                if (pos == str_view::npos) {
                    callback.on_scalar(line, anchor, tag);
                }
                else {
                    std::string scalar {};
                    while (pos != str_view::npos) {
                        scalar.append(line.begin(), line.begin() + pos);
                        line.remove_prefix(pos);

                        assert(line.size() > 1);
                        assert(line.starts_with('\\'));

                        switch (line[1]) {
                        case 'n':
                            scalar.push_back('\n');
                            break;
                        case 't':
                            scalar.push_back('\t');
                            break;
                        case '\\':
                            scalar.push_back('\\');
                            break;
                        default:
                            assert(false);
                        }

                        line.remove_prefix(2); // move after the escape sequence.

                        pos = line.find('\\');
                    }

                    callback.on_scalar(scalar, anchor, tag);
                }
            }
            else if (line.starts_with("=ALI")) {
                // this line should be "=ALI *anchor", and the "anchor" part must not be empty.
                assert(line.size() > 6);
                assert(line[5] == '*');

                str_view anchor = line.substr(6);
                callback.on_alias(anchor);
            }
        }
    }

    void parse_container_begin_attrs(str_view line, str_view flow_indicator, str_view& anchor, str_view& tag) {
        if (line.size() > 4) {
            line.remove_prefix(5); // skip "+SEQ "

            // The format of the "+(SEQ|MAP) ..." line is:
            //   +(SEQ|MAP) [] &anchor <tag>
            // "[]", "&anchor" and "<tag>" are all optional.

            if (line.starts_with(flow_indicator)) {
                if (line.size() > 2) {
                    line.remove_prefix(3); // move after the trailing space.
                }
                else {
                    line.remove_prefix(2);
                }
            }

            if (!line.empty()) {
                parse_anchor_and_tag(line, anchor, tag);
            }
        }
    }

    void parse_anchor_and_tag(str_view& line, str_view& anchor, str_view& tag) {
        if (line.starts_with('&')) {
            line.remove_prefix(1);
            std::size_t pos = line.find_first_of(' ');
            if (pos == str_view::npos) {
                anchor = line;
                line.remove_prefix(line.size());
                return;
            }

            anchor = line.substr(0, pos);
            line.remove_prefix(pos + 1);
        }

        if (line.starts_with('<')) {
            line.remove_prefix(1);
            std::size_t pos = line.find_first_of('>');
            assert(pos != str_view::npos);
            tag = line.substr(0, pos);

            line.remove_prefix(pos + 1); // move after the '>'.
            if (line.starts_with(' ')) {
                line.remove_prefix(1); // move after the trailing space
            }
        }
    }

private:
    stream_event_callback* mp_callback {nullptr};
};

#endif /* FK_YAML_YAML_TEST_SUITE_RUNNER_EVENT_STREAM_ENGINE_HPP */
