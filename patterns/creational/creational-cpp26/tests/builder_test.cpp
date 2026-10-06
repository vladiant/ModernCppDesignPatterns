/// \file builder_test.cpp
/// \brief Catch2 tests for Builder (C4): deducing-this fluent setters and a
///        validated build() returning std::expected.

#include <mcpp/creational26/builder.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <utility>

using namespace mcpp::creational26;

TEST_CASE("Happy path builds a complete request (rvalue chain moves out)",
          "[builder]") {
    auto result = HttpRequestBuilder{}
                      .with_method("POST")
                      .with_url("https://example.com")
                      .with_header("Accept", "application/json")
                      .with_body("payload")
                      .build();
    REQUIRE(result.has_value());
    REQUIRE(result->method == "POST");
    REQUIRE(result->url == "https://example.com");
    REQUIRE(result->headers.size() == 1);
    REQUIRE(result->headers.front().first == "Accept");
    REQUIRE(result->body == "payload");
}

TEST_CASE("Default method is GET", "[builder][edge]") {
    auto result = HttpRequestBuilder{}.with_url("https://example.com").build();
    REQUIRE(result.has_value());
    REQUIRE(result->method == "GET");
}

TEST_CASE("Missing url fails validation", "[builder][error]") {
    auto result = HttpRequestBuilder{}.with_method("GET").build();
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == BuildError::missing_url);
}

TEST_CASE("Invalid method fails validation", "[builder][error]") {
    auto result = HttpRequestBuilder{}
                      .with_method("FETCH")
                      .with_url("https://example.com")
                      .build();
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == BuildError::invalid_method);
}

TEST_CASE("Empty header name fails validation", "[builder][error]") {
    auto result = HttpRequestBuilder{}
                      .with_url("https://example.com")
                      .with_header("", "value")
                      .build();
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == BuildError::empty_header_name);
}

TEST_CASE("Deducing-this setters work on a named lvalue too", "[builder][edge]") {
    // The same setter body serves lvalues: mutate in place, then build from a
    // const lvalue (fields copied).
    HttpRequestBuilder builder;
    builder.with_method("PUT").with_url("https://example.com/resource");
    const HttpRequestBuilder& ref = builder;
    auto result = ref.build();
    REQUIRE(result.has_value());
    REQUIRE(result->method == "PUT");
    // Builder still usable after a const-lvalue build (fields were copied).
    REQUIRE(builder.build().has_value());
}
