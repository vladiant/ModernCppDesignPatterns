/// \file interpreter.hpp
/// \brief Visitor / Interpreter (C17) — the AST is a `std::variant`;
///        `std::visit` *is* the visitor. Recursion uses `gof::indirect<Expr>`.

#ifndef MCPP_BEHAVIORAL26_INTERPRETER_HPP
#define MCPP_BEHAVIORAL26_INTERPRETER_HPP

#include "compat.hpp"

#include <format>
#include <string>
#include <utility>
#include <variant>

namespace mcpp::behavioral26 {

struct Expr;

/// Leaf: a numeric literal.
struct Num {
    double value;
};

/// Internal node: a binary operator over two sub-expressions. The children are
/// `gof::indirect<Expr>` so the recursive type has value semantics.
struct BinOp {
    char op;
    gof::indirect<Expr> lhs, rhs;
};

struct Expr {
    std::variant<Num, BinOp> node;
};

/// Build a numeric-literal expression.
inline Expr num(double v) { return Expr{Num{v}}; }

/// Build a binary expression `op(l, r)`.
inline Expr bin(char op, Expr l, Expr r) {
    return Expr{BinOp{op, gof::indirect<Expr>(std::in_place, std::move(l)),
                      gof::indirect<Expr>(std::in_place, std::move(r))}};
}

/// Evaluate an expression to a double (a new "operation" = a new overload set).
inline double eval(const Expr& e) {
    return std::visit(
        gof::overloaded{
            [](const Num& n) { return n.value; },
            [](const BinOp& b) {
                const double l = eval(*b.lhs), r = eval(*b.rhs);
                switch (b.op) {
                    case '+': return l + r;
                    case '-': return l - r;
                    case '*': return l * r;
                    default: return l / r;
                }
            }},
        e.node);
}

/// Pretty-print an expression as a fully-parenthesized string.
inline std::string show(const Expr& e) {
    return std::visit(
        gof::overloaded{
            [](const Num& n) { return std::format("{}", n.value); },
            [](const BinOp& b) {
                return std::format("({} {} {})", show(*b.lhs), b.op,
                                   show(*b.rhs));
            }},
        e.node);
}

}  // namespace mcpp::behavioral26

#endif  // MCPP_BEHAVIORAL26_INTERPRETER_HPP
