# Abstract Factory — Creational — C++20

**Pattern:** Abstract Factory (GoF creational).
**Standard:** C++20.
**Showcased idioms:** `concepts` constraining product families, `std::format`.

## Problem

A UI toolkit must create matching **families** of widgets (a `Button` and a
`Checkbox`) for either a **Light** or a **Dark** theme, without the client code
knowing the concrete types.

## Design — the modern twist

The "abstract products" and "abstract factory" are **concepts**, not base
classes. There is **no virtual inheritance**; everything is value semantics and
resolved at compile time.

- `Button<T>` / `Checkbox<T>` — product concepts (require a `render()`).
- `WidgetFactory<F>` — factory concept (must `make_button()` + `make_checkbox()`
  whose results satisfy the product concepts).
- `LightTheme` / `DarkTheme` — concrete factories.
- `render_dialog(const F&)` — generic client constrained by `WidgetFactory<F>`.

A wrong-family mix or a non-factory argument (`render_dialog(int{})`) is a
**compile error**, so families cannot be mixed by accident.

## Build & run

```bash
cmake -S . -B build -DPATTERN_WERROR=ON
cmake --build build
ctest --test-dir build --output-on-failure
./build/abstract_factory_demo
```
