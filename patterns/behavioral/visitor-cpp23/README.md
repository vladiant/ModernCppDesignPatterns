# Visitor — Behavioral — C++23

**Pattern:** Visitor (GoF behavioral).
**Standard:** C++23.
**Showcased idioms:** `std::variant` + an `overloaded` visitor; recursion via
**deducing this**.

## Problem

A small arithmetic **expression tree** (`Number`, `Add`, `Mul`, `Neg`) that
supports multiple operations (`evaluate`, `to_string`) without baking a
`visit()` method into every node type.

## Design

- `Expr` wraps `std::variant<Number, Add, Mul, Neg>`; child edges are owning
  `ExprPtr = std::unique_ptr<Expr>` (RAII, value semantics in / `unique_ptr`
  out via the `number/add/mul/neg` factories).
- The classic `overloaded<Ts...>` helper (`using Ts::operator()...` + CTAD)
  dispatches over the variant alternatives.
- **Recursion** over child nodes uses a `[](this auto const& self, ...)`
  deducing-this lambda that calls *itself* — no `std::function`, no named
  recursive free function. Both `evaluate` and `to_string` use this idiom.

`to_string` produces a fully-parenthesized form, e.g.
`add(mul(number(3), number(4)), neg(number(5)))` → `((3 * 4) + (-5))`,
which `evaluate` reduces to `7`.

## Build & run

```bash
cmake -S . -B build -DPATTERN_WERROR=ON -DCMAKE_CXX_COMPILER=g++-14
cmake --build build
ctest --test-dir build --output-on-failure
./build/visitor_demo
```

> **Toolchain note:** this project uses *deducing this* (P0847), which requires
> **g++ ≥ 14** or **clang ≥ 18**. The default `g++` 13.3 does not support it —
> configure with `-DCMAKE_CXX_COMPILER=g++-14`.
