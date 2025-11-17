// String serialize/deserialize round-trip tests

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("String_Serialize_Deserialize_Roundtrip") {
    fkyaml::node source = fkyaml::node::mapping();
    source["alias"]      = "*anchor";
    source["anchor"]     = "&anchor";
    source["tag"]        = "!tag";
    source["comment"]    = "#not-a-comment";
    source["brackets"]   = "[1,2]";
    source["braces"]     = "{a: 1}";
    source["comma"]      = "a,b";
    source["pipe"]       = "a|b";
    source["gt"]         = "a>b";
    source["at"]         = "user@host";
    source["backtick"]   = "`code`";
    source["percent"]    = "100%";
    source["colon"]      = "key:value";
    source["question"]   = "?maybe";
    source["dash"]       = "-value";

    // Values that should *not* trigger scane_yaml_tokens
    source["plain"]      = "just_text";
    source["empty"]      = "";

    std::stringstream buff;
    buff << source;

    fkyaml::node copy;
    REQUIRE_NOTHROW(copy = fkyaml::node::deserialize(buff));
    for (const auto& kv : source.map_items()) {
        const auto& key  = kv.key().as_str();
        const auto& lhs  = source[key];
        const auto& rhs  = copy[key];

        REQUIRE(lhs == rhs);
    }
}
