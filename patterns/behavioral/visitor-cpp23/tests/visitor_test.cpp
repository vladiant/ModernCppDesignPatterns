/// \file visitor_test.cpp
/// \brief Catch2 tests for the Visitor pattern (variant + overloaded + deducing
///        this recursion).

#include <mcpp/visitor/expression.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace mcpp::visitor;
using Catch::Matchers::WithinRel;

TEST_CASE("evaluate handles leaves and binary nodes", "[visitor][evaluate]") {
    REQUIRE_THAT(evaluate(*number(42)), WithinRel(42.0));
    REQUIRE_THAT(evaluate(*add(number(2), number(3))), WithinRel(5.0));
    REQUIRE_THAT(evaluate(*mul(number(3), number(4))), WithinRel(12.0));
    REQUIRE_THAT(evaluate(*neg(number(5))), WithinRel(-5.0));
}

TEST_CASE("evaluate handles nesting and Neg", "[visitor][evaluate][nested]") {
    // (3 * 4) + (-5) = 7
    auto tree = add(mul(number(3), number(4)), neg(number(5)));
    REQUIRE_THAT(evaluate(*tree), WithinRel(7.0));
}

TEST_CASE("evaluate handles a deeply nested tree", "[visitor][evaluate][deep]") {
    // -(((1 + 1) * (2 + 2)) * 3) = -24
    auto tree = neg(mul(mul(add(number(1), number(1)), add(number(2), number(2))), number(3)));
    REQUIRE_THAT(evaluate(*tree), WithinRel(-24.0));
}

TEST_CASE("to_string matches the documented fully-parenthesized format",
          "[visitor][to_string]") {
    auto tree = add(mul(number(3), number(4)), neg(number(5)));
    REQUIRE(to_string(*tree) == "((3 * 4) + (-5))");

    REQUIRE(to_string(*number(7)) == "7");
    REQUIRE(to_string(*neg(number(2))) == "(-2)");
}

TEST_CASE("Factories transfer ownership into the tree", "[visitor][ownership]") {
    // Moving an ExprPtr into a factory transfers ownership; the moved-from
    // pointer is null, and the resulting tree still evaluates correctly.
    ExprPtr leaf = number(9);
    REQUIRE(leaf != nullptr);
    ExprPtr negated = neg(std::move(leaf));
    REQUIRE(leaf == nullptr);               // ownership moved out
    REQUIRE_THAT(evaluate(*negated), WithinRel(-9.0));
}
