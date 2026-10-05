/// \file main.cpp
/// \brief Visitor demo: arithmetic expression tree via std::variant +
///        overloaded + deducing-this recursion.

#include <mcpp/visitor/expression.hpp>

#include <format>
#include <iostream>

int main() {
    using namespace mcpp::visitor;

    // (3 * 4) + (-5)  ->  to_string "((3 * 4) + (-5))", evaluate 7
    auto tree = add(mul(number(3), number(4)), neg(number(5)));

    std::cout << std::format("expr   = {}\n", to_string(*tree));
    std::cout << std::format("value  = {}\n", evaluate(*tree));

    // A deeper tree: -( (2 + 3) * 4 ) = -20
    auto deep = neg(mul(add(number(2), number(3)), number(4)));
    std::cout << std::format("expr2  = {}\n", to_string(*deep));
    std::cout << std::format("value2 = {}\n", evaluate(*deep));

    return 0;
}
