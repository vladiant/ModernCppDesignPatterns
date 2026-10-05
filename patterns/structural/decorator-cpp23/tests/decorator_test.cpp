/// \file decorator_test.cpp
/// \brief Catch2 tests for the Decorator pattern (deducing this + static call).

#include <mcpp/decorator/pipeline.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace mcpp::decorator;

TEST_CASE("Stateless layers transform correctly in isolation", "[decorator][layer]") {
    REQUIRE(Uppercase{}("abc") == "ABC");
    REQUIRE(Trim{}("  padded  ") == "padded");
    REQUIRE(Trim{}("   ") == "");
    REQUIRE(Prefix{"[LOG] "}("msg") == "[LOG] msg");
}

TEST_CASE("Concept classifies layers", "[decorator][concept]") {
    STATIC_REQUIRE(Layer<Uppercase>);
    STATIC_REQUIRE(Layer<Trim>);
    STATIC_REQUIRE(Layer<Prefix>);
    STATIC_REQUIRE_FALSE(Layer<int>);
}

TEST_CASE("Composed pipeline applies layers in documented order", "[decorator][compose]") {
    auto pipeline = decorate(Trim{}).with(Uppercase{}).with(Prefix{"[LOG] "});
    // Identity -> Trim -> Uppercase -> Prefix
    REQUIRE(pipeline("  hello  ") == "[LOG] HELLO");
}

TEST_CASE("Composition equals the manual nested application", "[decorator][equiv]") {
    const std::string in = "  Mixed Case  ";

    auto composed = decorate(Trim{}).with(Uppercase{}).with(Prefix{">> "});
    const std::string manual = Prefix{">> "}(Uppercase{}(Trim{}(in)));

    REQUIRE(composed(in) == manual);
}

TEST_CASE("The composite is a reusable value type", "[decorator][value]") {
    auto pipeline = decorate(Uppercase{});
    auto copy = pipeline;
    REQUIRE(pipeline("ab") == "AB");
    REQUIRE(copy("cd") == "CD");
}
