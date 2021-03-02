// Copyright 2013 Daniel Parker
// Distributed under Boost license

#include <jsoncons/json.hpp>
#include <jsoncons/json_encoder.hpp>
#include <jsoncons/json_reader.hpp>
#include "sample_allocators.hpp"
#include <catch/catch.hpp>
#include <sstream>
#include <vector>
#include <utility>
#include <ctime>

using namespace jsoncons;

TEST_CASE("json_raw_reader read from string test")
{
    std::string s = R"(
{
  "store": {
    "book": [
      {
        "category": "reference",
        "author": "Margaret Weis",
        "title": "Dragonlance Series",
        "price": 31.96
      },
      {
        "category": "reference",
        "author": "Brent Weeks",
        "title": "Night Angel Trilogy",
        "price": 14.70
      }
    ]
  }
}
)";

    SECTION("test 1")
    {
        json_decoder<json> decoder;
        basic_json_raw_reader<char> reader(s, decoder);
        reader.read();
        json j = decoder.get_result();

        REQUIRE(j.is_object());
        REQUIRE(j.size() == 1);
        REQUIRE(j[0].is_object());
        REQUIRE(j[0].size() == 1);
        REQUIRE(j[0][0].is_array());
        REQUIRE(j[0][0].size() == 2);
        CHECK(j[0][0][0]["category"].as<std::string>() == std::string("reference"));
        CHECK(j[0][0][1]["author"].as<std::string>() == std::string("Brent Weeks"));
    }
}



