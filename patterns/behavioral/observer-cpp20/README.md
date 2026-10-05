# Observer — Behavioral — C++20

**Pattern:** Observer (GoF behavioral).
**Standard:** C++20.
**Showcased idioms:** `operator<=>` for subscriber ordering/dedup,
`std::ranges` for dispatch, `std::format`.

## Problem

A `Subject` broadcasts `Event`s to many `Observer`s. Observers should fire in a
deterministic order and the same subscriber must not register twice.

## Design

- `Observer` carries `(priority, name, callback)`. Its **`operator<=>`**
  (`std::strong_ordering`) compares by `(priority ascending, name)` — the
  callback is deliberately excluded from identity. This single comparison drives
  **both** dispatch order **and** dedup.
- `Subject` keeps a `std::vector<Observer>` **sorted** (insertion via
  `std::ranges::lower_bound`); `subscribe` rejects an equal observer;
  `publish` dispatches with `std::ranges::for_each` in `<=>` order.

Ownership: `Subject` owns `Observer` values. Callbacks are `std::function` and
may capture external state by reference, which must outlive `notify()`.

## Build & run

```bash
cmake -S . -B build -DPATTERN_WERROR=ON
cmake --build build
ctest --test-dir build --output-on-failure
./build/observer_demo
```
