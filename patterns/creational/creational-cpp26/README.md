# Creational Patterns — C++26 Idiom Tier

**Group:** Creational (GoF) — patterns **C1–C5**.
**Standard:** C++26 (`-std=c++26`).
**Toolchain:** g++-14 on Ubuntu 24.04. clang-18 and g++-13 **cannot** build this
tier (they lack the required C++26 facilities) — configure with
`-DCMAKE_CXX_COMPILER=g++-14`.

One standalone, header-only project that narrates five creational patterns, each
rewritten around a modern C++26 idiom that retires its classic GoF boilerplate.
Pattern logic lives in `include/mcpp/creational26/*.hpp`; `app/main.cpp` is a thin
narrative demo; `tests/` holds Catch2 v3 unit tests.

## Patterns & idioms (C1–C5)

| # | Pattern | C++26 idiom (visible) | Retires |
|---|---------|-----------------------|---------|
| C1 | **Singleton** | `= delete("reason")` on copy/move + a Meyers-style accessor | silent/cryptic deleted-copy errors |
| C2 | **Factory Method** | `std::move_only_function` creators in a registry; `create()` → `std::expected<Shape, FactoryError>` | `std::function`, null returns, exceptions |
| C3 | **Abstract Factory** | `concept WidgetFactory` over *theme* types; generic constrained client | a virtual factory hierarchy |
| C4 | **Builder** | *deducing this* fluent setters (one body for `&`/`&&`); validated `build()` → `std::expected` | duplicated `&`/`&&` setter overloads |
| C5 | **Prototype** | `gof::polymorphic<Figure>` registry — copy **is** deep clone | a virtual `clone()` |

### One-liner per pattern

- **C1 Singleton** — `AppConfig::instance()` is the single process-wide object;
  its copy/move operations are `= delete("… take a const& instead …")` so a copy
  attempt yields a diagnostic that *names the fix* (see the commented line in the
  demo). Deletion is observable in tests via `std::is_copy_constructible_v` etc.
- **C2 Factory Method** — `ShapeFactory` maps a name → a move-only creator
  closure; `create("hexagon", size)` returns a `std::expected<Shape, …>` and an
  unknown key returns `std::unexpected(FactoryError::unknown_kind)` — never null,
  never a throw.
- **C3 Abstract Factory** — `LightTheme` / `DarkTheme` are plain structs with no
  common base; the `WidgetFactory` **concept** is the target interface, and the
  generic `render_dialog(factory)` works with whichever conforming family it is
  handed (non-conformance is a compile error).
- **C4 Builder** — `HttpRequestBuilder`'s setters are
  `template <class Self> auto&& with_x(this Self&& self, …)`, so one body serves
  both lvalue and rvalue chains and preserves value category (an rvalue chain
  moves its fields straight into the product); `build()` validates and returns
  `std::expected<HttpRequest, BuildError>`.
- **C5 Prototype** — a `PrototypeRegistry` stores `gof::polymorphic<Figure>`
  prototypes; cloning is a **plain value copy** of the wrapper, which deep-clones
  the stored dynamic type (`Circle`, `Rectangle`) with no virtual `clone()`.

## Active compatibility shims (on g++-14 `-std=c++26`)

This project copies the proven `compat.hpp` shim (namespace `gof`, DESIGN
§C26.3) byte-for-byte from the behavioral tier. On g++-14 the relevant state is:

- **`gof::polymorphic<T>` — fallback active** (`__cpp_lib_polymorphic` is **not**
  defined on g++-14). Used by **C5**. The fallback captures copier/deleter
  function pointers from the concrete `U` at the construction site, so copying
  deep-clones the dynamic type without a virtual `clone()`. It is a demonstration
  aid, not a production reimplementation (SRS A-C26-2): copy-constructible,
  complete-at-construction `U` only; no allocator / `constexpr` support.
- **`= delete("reason")` — reason string guarded** (C1). The deleted-function
  *reason* (P2573) requires `__cpp_deleted_function >= 202403L`, which g++-14 does
  **not** yet define (GCC 15+). `singleton.hpp` guards it with `MCPP_DELETE_MSG`:
  the reason text is always present in the source (the idiom is visible), and the
  compiler surfaces it verbatim where supported, degrading to a plain `= delete`
  on g++-14 without changing behavior.

Facilities used **directly** (present on g++-14, no shim): `std::expected`,
`std::move_only_function`, *deducing this* (P0847), and concepts.

> **Note on `std::to_string`:** under C++26 (P2587) `std::to_string` on a
> floating-point value uses the shortest round-trip form, so e.g. `2.0` renders
> as `"2"`. The demo output and tests reflect this.

## Build & run

```bash
cmake -S . -B build -DCMAKE_CXX_COMPILER=g++-14 -DPATTERN_WERROR=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/creational26_demo
```

Targets (DD-C26-10): library `creational26` (INTERFACE, header-only), demo
`creational26_demo`, tests `creational26_tests`. Catch2 v3 is reused if found,
else fetched (`v3.7.1`); `catch_discover_tests` registers one CTest entry per
`TEST_CASE`. The build is warning-free under `-Wall -Wextra -Wpedantic -Werror`.
