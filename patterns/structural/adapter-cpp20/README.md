# Adapter — Structural — C++20

**Pattern:** Adapter (GoF structural).
**Standard:** C++20.
**Showcased idioms:** `concepts` + `std::ranges`/views to adapt an incompatible
interface.

## Problem

A **legacy sensor** exposes an index-based, Fahrenheit, non-range API
(`count()` + `fahrenheit_at(i)`). Modern code wants a lazy `std::ranges` view of
**Celsius** values that composes with standard range adaptors.

## Design

- `IndexedFahrenheitSource<S>` — concept describing any wrappable source, so the
  adapter is reusable (not tied to `LegacySensor`).
- `LegacySensor` — the adaptee (throws `std::out_of_range` on bad index).
- `CelsiusView<S>` — the adapter. `celsius()` returns
  `views::iota(0..count) | views::transform(index -> celsius)`.

The returned view is **lazy** and holds a **non-owning** reference to the
source, which must outlive the view. Iterating twice re-reads the source.

## Build & run

```bash
cmake -S . -B build -DPATTERN_WERROR=ON
cmake --build build
ctest --test-dir build --output-on-failure
./build/adapter_demo
```
