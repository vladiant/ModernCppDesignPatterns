/// \file interpreter_test.cpp
/// \brief Catch2 tests for Visitor/Interpreter (C17): AST eval + show, with
///        gof::indirect recursion.

#include <mcpp/behavioral26/interpreter.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace mcpp::behavioral26;

TEST_CASE("eval computes (1+2)*(10-4) == 18", "[interpreter]") {
    Expr e = bin('*', bin('+', num(1), num(2)), bin('-', num(10), num(4)));
    REQUIRE(eval(e) == 18.0);
}

TEST_CASE("show pretty-prints the fully-parenthesized form", "[interpreter]") {
    Expr e = bin('*', bin('+', num(1), num(2)), bin('-', num(10), num(4)));
    REQUIRE(show(e) == "((1 + 2) * (10 - 4))");
}

TEST_CASE("each operator evaluates correctly", "[interpreter]") {
    REQUIRE(eval(bin('+', num(3), num(4))) == 7.0);
    REQUIRE(eval(bin('-', num(3), num(4))) == -1.0);
    REQUIRE(eval(bin('*', num(3), num(4))) == 12.0);
    REQUIRE(eval(bin('/', num(12), num(4))) == 3.0);
}

TEST_CASE("a bare literal evaluates to itself", "[interpreter][edge]") {
    REQUIRE(eval(num(42)) == 42.0);
    REQUIRE(show(num(42)) == "42");
}

TEST_CASE("indirect children give the AST value semantics", "[interpreter]") {
    Expr original = bin('+', num(1), num(2));
    Expr copy = original;  // deep copy via gof::indirect
    REQUIRE(eval(copy) == 3.0);
    REQUIRE(eval(original) == 3.0);  // original intact
}
