#ifndef MCPP_VISITOR_EXPRESSION_HPP
#define MCPP_VISITOR_EXPRESSION_HPP

/// \file expression.hpp
/// \brief Visitor pattern over a recursive expression tree (C++23).
///
/// Showcased idioms:
///  - **`std::variant`** node + the classic **`overloaded`** visitor.
///  - **deducing this** recursion: a `[](this auto const& self, ...)` lambda
///    calls itself without `std::function` or a named recursive free function.

#include <format>
#include <memory>
#include <string>
#include <utility>
#include <variant>

namespace mcpp::visitor {

struct Expr; // forward decl (recursive)

/// Owning child edge.
using ExprPtr = std::unique_ptr<Expr>;

/// Leaf: a literal number.
struct Number {
    double value;
};

/// Binary addition.
struct Add {
    ExprPtr lhs;
    ExprPtr rhs;
};

/// Binary multiplication.
struct Mul {
    ExprPtr lhs;
    ExprPtr rhs;
};

/// Unary negation.
struct Neg {
    ExprPtr operand;
};

/// An expression node: a discriminated union of the alternatives above.
struct Expr {
    std::variant<Number, Add, Mul, Neg> node;
};

/// Classic overloaded helper (aggregate + inherited call operators + CTAD).
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

// ----- factory helpers (value semantics in, unique_ptr out) -----

[[nodiscard]] inline ExprPtr number(double v) {
    return std::make_unique<Expr>(Expr{Number{v}});
}

[[nodiscard]] inline ExprPtr add(ExprPtr a, ExprPtr b) {
    return std::make_unique<Expr>(Expr{Add{std::move(a), std::move(b)}});
}

[[nodiscard]] inline ExprPtr mul(ExprPtr a, ExprPtr b) {
    return std::make_unique<Expr>(Expr{Mul{std::move(a), std::move(b)}});
}

[[nodiscard]] inline ExprPtr neg(ExprPtr a) {
    return std::make_unique<Expr>(Expr{Neg{std::move(a)}});
}

/// Evaluate the expression tree to a numeric value.
/// Recursion uses a deducing-this self-recursive lambda.
[[nodiscard]] inline double evaluate(const Expr& e) {
    auto eval = [](this auto const& self, const Expr& node) -> double {
        return std::visit(overloaded{
                              [](const Number& n) { return n.value; },
                              [&](const Add& a) { return self(*a.lhs) + self(*a.rhs); },
                              [&](const Mul& m) { return self(*m.lhs) * self(*m.rhs); },
                              [&](const Neg& g) { return -self(*g.operand); },
                          },
                          node.node);
    };
    return eval(e);
}

/// Pretty-print the expression as a fully-parenthesized string.
/// Recursion uses the same deducing-this idiom.
[[nodiscard]] inline std::string to_string(const Expr& e) {
    auto str = [](this auto const& self, const Expr& node) -> std::string {
        return std::visit(
            overloaded{
                [](const Number& n) { return std::format("{}", n.value); },
                [&](const Add& a) {
                    return std::format("({} + {})", self(*a.lhs), self(*a.rhs));
                },
                [&](const Mul& m) {
                    return std::format("({} * {})", self(*m.lhs), self(*m.rhs));
                },
                [&](const Neg& g) { return std::format("(-{})", self(*g.operand)); },
            },
            node.node);
    };
    return str(e);
}

} // namespace mcpp::visitor

#endif // MCPP_VISITOR_EXPRESSION_HPP
