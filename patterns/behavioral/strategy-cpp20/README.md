# Strategy — Behavioral — C++20

**Pattern:** Strategy (GoF behavioral).
**Standard:** C++20.
**Showcased idioms:** `std::invocable`/concepts, `std::span` over input,
compile-time strategy selection (no virtual dispatch).

## Problem

A numeric **reducer** collapses a sequence of `double`s to a single value. The
*how* (sum, mean, max, …) must be pluggable without changing the reducer.

## Design

- `ReduceStrategy<S>` — a concept: any invocable with the shape
  `double(std::span<const double>)` qualifies, including plain lambdas.
- `Sum`, `Mean`, `Max` — concrete strategies (value types, `operator()`).
- `reduce<S>(data, strategy)` — the context; the strategy type is a **template
  parameter**, so selection is resolved at compile time.

### Empty-span policy

| Strategy | Empty input |
|----------|-------------|
| `Sum`    | `0.0`       |
| `Mean`   | `0.0`       |
| `Max`    | throws `std::invalid_argument` (programmer error; return stays plain `double`) |

## Build & run

```bash
cmake -S . -B build -DPATTERN_WERROR=ON
cmake --build build
ctest --test-dir build --output-on-failure
./build/strategy_demo
```
