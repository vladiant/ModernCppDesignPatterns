/// \file main.cpp
/// \brief Command demo: undoable accumulator with std::expected + if consteval.

#include <mcpp/command/command_stack.hpp>
#include <mcpp/command/commands.hpp>

#include <format>
#include <iostream>
#include <string>

namespace {
using namespace mcpp::command;

std::string describe(CommandError e) {
    switch (e) {
    case CommandError::divide_by_zero:
        return "divide_by_zero";
    case CommandError::nothing_to_undo:
        return "nothing_to_undo";
    case CommandError::overflow:
        return "overflow";
    }
    return "unknown";
}

void report(const std::string& label, const std::expected<void, CommandError>& r,
            const Accumulator& acc) {
    if (r) {
        std::cout << std::format("{:<16} -> value = {}\n", label, acc.value);
    } else {
        std::cout << std::format("{:<16} -> ERROR {} (value = {})\n", label,
                                 describe(r.error()), acc.value);
    }
}
} // namespace

int main() {
    using namespace mcpp::command;

    // `if consteval`: known-at-compile-time division is checked at compile time.
    static_assert(safe_div(10, 2).value() == 5);
    // The next line would be a *compile error* (division by zero in constant
    // evaluation), demonstrating `if consteval`:
    //   static_assert(safe_div(1, 0).has_value());

    Accumulator acc;
    CommandStack stack;

    AddCommand add5{5};
    report("Add 5", stack.run([&](Accumulator& a) { return add5.execute(a); },
                              [&](Accumulator& a) { return add5.undo(a); }, acc),
           acc);

    report("Negate", stack.run(Negate{}, Negate{}, acc), acc);

    DivideCommand div0{0};
    report("Divide by 0", stack.run([&](Accumulator& a) { return div0.execute(a); },
                                    [&](Accumulator& a) { return div0.undo(a); }, acc),
           acc); // runtime std::expected error branch; nothing recorded

    AddCommand add2{2};
    report("Add 2", stack.run([&](Accumulator& a) { return add2.execute(a); },
                              [&](Accumulator& a) { return add2.undo(a); }, acc),
           acc);

    std::cout << std::format("history depth = {}\n", stack.depth());

    report("Undo", stack.undo(acc), acc); // undo Add 2
    report("Undo", stack.undo(acc), acc); // undo Negate

    return 0;
}
