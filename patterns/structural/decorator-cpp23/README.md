# Decorator — Structural — C++23

**Pattern:** Decorator (GoF structural).
**Standard:** C++23.
**Showcased idioms:** **deducing this** for recursive, type-preserving
composition; **`static operator()`** for stateless decorator layers.

## Problem

A **text-rendering pipeline** where each *layer* transforms
`std::string -> std::string`. Layers must compose in order, and the composition
should stay a concrete type (no type erasure / `std::function` in the hot path).

## Design

- `Layer<L>` — concept: any invocable `std::string(std::string)`.
- `Uppercase`, `Trim` — stateless layers implemented with **`static
  operator()`** (no per-object state).
- `Prefix{tag}` — a stateful layer (non-static `operator()` reading `tag`).
- `Decorated<Inner, L>` — composition. Applies `Inner` first, then `L`.
  `operator()(this const Decorated&, std::string)` and
  `with(this Self&&, Next)` use **deducing this**, so each `.with(...)` grows a
  new, fully-typed composite `Decorated<Decorated<...>, Next>`.
- `decorate(src)` — seeds a pipeline with an identity base + first layer.

Application order for `decorate(A).with(B).with(C)` is **A, then B, then C**.

## Build & run

```bash
cmake -S . -B build -DPATTERN_WERROR=ON -DCMAKE_CXX_COMPILER=g++-14
cmake --build build
ctest --test-dir build --output-on-failure
./build/decorator_demo
```

> **Toolchain note:** this project uses *deducing this* (P0847), which requires
> **g++ ≥ 14** or **clang ≥ 18**. The environment's default `g++` is 13.3, which
> does not support it — configure with `-DCMAKE_CXX_COMPILER=g++-14`.
