# Command — Behavioral — C++23

**Pattern:** Command (GoF behavioral).
**Standard:** C++23.
**Showcased idioms:** `std::expected` for `execute()`/`undo()`,
**`static operator()`** for a stateless command, **`if consteval`** for a
compile-time-validated division.

## Problem

An integer **accumulator** (tiny calculator) with an **undoable** command stack.
Commands mutate the accumulator and report success/failure via `std::expected`.

## Design

- `CommandError` — `divide_by_zero`, `nothing_to_undo`, `overflow`.
- `Accumulator` — the mutable receiver.
- `safe_div(a, b)` — `constexpr` helper using **`if consteval`**: a
  divide-by-zero is a **hard compile error** when operands are known at compile
  time, but a **runtime `std::expected` error** otherwise.
- `Command<C>` — concept: `execute`/`undo` each return
  `std::expected<void, CommandError>`.
- `AddCommand` (overflow-guarded), `DivideCommand` (remembers previous for undo,
  divides via `safe_div`), `Negate` (stateless, **`static operator()`**).
- `CommandStack` — the invoker; owns a `std::function` undo history
  (type-erased so it stays non-templated and testable). `undo` on an empty
  stack returns `nothing_to_undo`; a failed command is **not** recorded.

## Build & run

```bash
cmake -S . -B build -DPATTERN_WERROR=ON -DCMAKE_CXX_COMPILER=g++-14
cmake --build build
ctest --test-dir build --output-on-failure
./build/command_demo
```

> **Toolchain note:** `std::expected` + `if consteval` build cleanly on
> **g++ ≥ 13**. g++-14 is used here for consistency with the other C++23
> projects. (clang-18 + the bundled libstdc++ hides `std::expected` because
> clang reports `__cpp_concepts == 201907L`.)
