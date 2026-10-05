/// \file builder_test.cpp
/// \brief Catch2 tests for the Builder pattern (deducing this + std::expected).

#include <mcpp/builder/request_builder.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace mcpp::builder;

TEST_CASE("Valid fluent chain builds the expected request", "[builder][happy]") {
    RequestBuilder b;
    auto result = b.method("POST")
                      .url("https://example.com/api")
                      .header("Accept", "application/json")
                      .body("payload")
                      .build();

    REQUIRE(result.has_value());
    REQUIRE(result->method == "POST");
    REQUIRE(result->url == "https://example.com/api");
    REQUIRE(result->headers.size() == 1);
    REQUIRE(result->headers.front().first == "Accept");
    REQUIRE(result->body == "payload");
}

TEST_CASE("Default method is GET", "[builder][default]") {
    auto result = RequestBuilder{}.url("https://example.com").build();
    REQUIRE(result.has_value());
    REQUIRE(result->method == "GET");
}

TEST_CASE("Missing url fails with missing_url", "[builder][error]") {
    auto result = RequestBuilder{}.method("GET").build();
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == BuildError::missing_url);
}

TEST_CASE("Unsupported method fails with invalid_method", "[builder][error]") {
    auto result = RequestBuilder{}.method("TELEPORT").url("https://example.com").build();
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == BuildError::invalid_method);
}

TEST_CASE("Empty header name fails with empty_header_name", "[builder][error]") {
    auto result = RequestBuilder{}.url("https://example.com").header("", "value").build();
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == BuildError::empty_header_name);
}

TEST_CASE("Lvalue chaining preserves value category (deducing this)",
          "[builder][deducing-this][lvalue]") {
    // The design states setters called on an lvalue return lvalue refs to the
    // SAME builder (not a copy). Verify reference identity on the lvalue branch,
    // which the rvalue-only test above does not cover.
    RequestBuilder b;
    auto& r1 = b.method("PUT");
    REQUIRE(&r1 == &b);
    auto& r2 = r1.url("https://example.com/x");
    REQUIRE(&r2 == &b);

    // The same lvalue builder accumulates state across separate statements and
    // can be built from (const-lvalue build() overload) without being consumed.
    b.header("A", "1");
    b.header("B", "2");
    auto result = b.build();
    REQUIRE(result.has_value());
    REQUIRE(result->method == "PUT");
    REQUIRE(result->headers.size() == 2);
    REQUIRE(result->headers[0].first == "A");
    REQUIRE(result->headers[1].first == "B");

    // build() on a const lvalue copies (does not consume): builder still usable.
    auto again = b.build();
    REQUIRE(again.has_value());
    REQUIRE(again->headers.size() == 2);
}

TEST_CASE("Rvalue-chained temporary builds successfully", "[builder][deducing-this]") {
    // Proves the deducing-this ref-category preservation compiles and works:
    // chaining on a temporary returns rvalues, ending in the rvalue build().
    auto result = RequestBuilder{}.method("DELETE").url("https://example.com/item/1").build();
    REQUIRE(result.has_value());
    REQUIRE(result->method == "DELETE");
    REQUIRE(result->url == "https://example.com/item/1");
}
