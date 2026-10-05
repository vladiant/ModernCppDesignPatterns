/// \file command_test.cpp
/// \brief Catch2 tests for the Command pattern.

#include <mcpp/command/command_stack.hpp>
#include <mcpp/command/commands.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace mcpp::command;

TEST_CASE("Commands satisfy the Command concept", "[command][concept]") {
    STATIC_REQUIRE(Command<AddCommand>);
    STATIC_REQUIRE(Command<DivideCommand>);
}

TEST_CASE("AddCommand updates and undoes", "[command][add]") {
    Accumulator acc{10};
    AddCommand add{5};

    REQUIRE(add.execute(acc).has_value());
    REQUIRE(acc.value == 15);
    REQUIRE(add.undo(acc).has_value());
    REQUIRE(acc.value == 10);
}

TEST_CASE("AddCommand reports overflow", "[command][add][edge]") {
    Accumulator acc{std::numeric_limits<long long>::max()};
    AddCommand add{1};
    auto r = add.execute(acc);
    REQUIRE_FALSE(r.has_value());
    REQUIRE(r.error() == CommandError::overflow);
    REQUIRE(acc.value == std::numeric_limits<long long>::max()); // unchanged
}

TEST_CASE("DivideCommand divides and undoes", "[command][divide]") {
    Accumulator acc{20};
    DivideCommand div{4};

    REQUIRE(div.execute(acc).has_value());
    REQUIRE(acc.value == 5);
    REQUIRE(div.undo(acc).has_value());
    REQUIRE(acc.value == 20);
}

TEST_CASE("DivideCommand by zero returns divide_by_zero at runtime", "[command][divide][error]") {
    Accumulator acc{20};
    DivideCommand div{0};
    auto r = div.execute(acc);
    REQUIRE_FALSE(r.has_value());
    REQUIRE(r.error() == CommandError::divide_by_zero);
    REQUIRE(acc.value == 20); // unchanged
}

TEST_CASE("Negate is a stateless static-call command", "[command][negate]") {
    Accumulator acc{7};
    REQUIRE(Negate{}(acc).has_value());
    REQUIRE(acc.value == -7);
}

TEST_CASE("CommandStack undo reverses applied commands", "[command][stack]") {
    Accumulator acc{0};
    CommandStack stack;
    AddCommand add3{3};
    AddCommand add4{4};

    REQUIRE(stack.run([&](Accumulator& a) { return add3.execute(a); },
                      [&](Accumulator& a) { return add3.undo(a); }, acc)
                .has_value());
    REQUIRE(stack.run([&](Accumulator& a) { return add4.execute(a); },
                      [&](Accumulator& a) { return add4.undo(a); }, acc)
                .has_value());
    REQUIRE(acc.value == 7);
    REQUIRE(stack.depth() == 2);

    REQUIRE(stack.undo(acc).has_value()); // undo add4
    REQUIRE(acc.value == 3);
    REQUIRE(stack.undo(acc).has_value()); // undo add3
    REQUIRE(acc.value == 0);
}

TEST_CASE("Undo on empty stack returns nothing_to_undo", "[command][stack][edge]") {
    Accumulator acc{0};
    CommandStack stack;
    auto r = stack.undo(acc);
    REQUIRE_FALSE(r.has_value());
    REQUIRE(r.error() == CommandError::nothing_to_undo);
}

TEST_CASE("Failed command is not recorded in history", "[command][stack][error]") {
    Accumulator acc{10};
    CommandStack stack;
    DivideCommand div0{0};
    auto r = stack.run([&](Accumulator& a) { return div0.execute(a); },
                       [&](Accumulator& a) { return div0.undo(a); }, acc);
    REQUIRE_FALSE(r.has_value());
    REQUIRE(stack.depth() == 0); // nothing pushed
}

TEST_CASE("safe_div constant-evaluation behavior (if consteval)", "[command][consteval]") {
    STATIC_REQUIRE(safe_div(10, 2).value() == 5);
    STATIC_REQUIRE(safe_div(9, 3).value() == 3);

    // Runtime divide-by-zero is a recoverable error, not a compile error.
    volatile long long zero = 0;
    auto r = safe_div(1, static_cast<long long>(zero));
    REQUIRE_FALSE(r.has_value());
    REQUIRE(r.error() == CommandError::divide_by_zero);
}
