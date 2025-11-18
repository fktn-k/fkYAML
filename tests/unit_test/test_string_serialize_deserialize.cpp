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

    std::stringstream buff;
    buff << source;

    fkyaml::node copy;
    REQUIRE_NOTHROW(copy = fkyaml::node::deserialize(buff));
    REQUIRE(source["alias"]     == copy["alias"]);
    REQUIRE(source["anchor"]    == copy["anchor"]);
    REQUIRE(source["tag"]       == copy["tag"]);
    REQUIRE(source["comment"]   == copy["comment"]);
    REQUIRE(source["brackets"]  == copy["brackets"]);
    REQUIRE(source["braces"]    == copy["braces"]);
    REQUIRE(source["comma"]     == copy["comma"]);
    REQUIRE(source["pipe"]      == copy["pipe"]);
    REQUIRE(source["gt"]        == copy["gt"]);
    REQUIRE(source["at"]        == copy["at"]);
    REQUIRE(source["backtick"]  == copy["backtick"]);
    REQUIRE(source["percent"]   == copy["percent"]);
    REQUIRE(source["colon"]     == copy["colon"]);
    REQUIRE(source["question"]  == copy["question"]);
    REQUIRE(source["dash"]      == copy["dash"]);
    REQUIRE(source["plain"]     == copy["plain"]);
}
