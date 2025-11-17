// String serialize/deserialize round-trip tests

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("String_Serialize_Deserialize_Roundtrip") {
    fkyaml::node source = fkyaml::node::mapping();
    source["pass"] = "***";
    source["host"] = "localhost:7007";

    std::stringstream buff;
    buff << source;

    fkyaml::node copy;
    REQUIRE_NOTHROW(copy = fkyaml::node::deserialize(buff));
    REQUIRE(source["pass"] == copy["pass"]);
    REQUIRE(source["host"] == copy["host"]);
}
