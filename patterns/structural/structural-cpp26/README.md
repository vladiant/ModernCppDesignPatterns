# Structural Patterns — C++26 Idiom Tier

**Project:** `structural_cpp26` (directory `patterns/structural/structural-cpp26/`)
**Patterns:** C6 Adapter · C7 Bridge · C8 Composite · C9 Decorator ·
C10 Facade · C11 Flyweight · C12 Proxy.
**Standard:** C++26 (`-std=c++26`), built **only** with **g++-14**.

This is the grouped C++26 structural project. Pattern logic is header-only (one
header per pattern in `include/mcpp/structural26/`); `app/main.cpp` is a thin
narrative demo and `tests/` holds the Catch2 translation units. It reuses the
shared compatibility shim `compat.hpp` (namespace `gof`), copied byte-for-byte
from the behavioral project (DESIGN §C26.3, DD-C26-3).

## Showcased C++26 idioms

| # | Pattern | Visible idiom | Replaces |
|---|---------|---------------|----------|
| C6 | Adapter | a `concept` (`TemperatureSource`) **is** the target interface; `FahrenheitAdapter` wraps a legacy sensor to satisfy it | abstract target base class |
| C7 | Bridge | `gof::polymorphic<Renderer>` member held by value; `Window` copy deep-copies its implementor | raw/`unique_ptr` to implementor |
| C8 | Composite | `std::variant` tree (`File`/`Directory`) + self-recursive `[](this auto&& self, …)` lambda over `std::visit` | virtual `Component` tree |
| C9 | Decorator | each layer owned as `gof::polymorphic<Notifier>`; copying a stack deep-clones the whole chain | manual `clone()` plumbing |
| C10 | Facade | `OrderFacade::place` chains `validate → charge → ship` with `std::expected::and_then` / `::transform` | nested if-error checks |
| C11 | Flyweight | `GlyphCache` interns immutable glyphs as `std::shared_ptr<const Glyph>`; repeats share one instance | (n/a) |
| C12 | Proxy | `std::call_once` + `std::once_flag` lazily materialise the expensive `RealImage` exactly once | hand-rolled bool flag + mutex |

## `compat.hpp` — the shared shim (namespace `gof`)

`compat.hpp` selects the real standard type when its feature-test macro is
defined, else provides a minimal honest fallback. The shim is a demonstration
aid, not a production reimplementation (SRS A-C26-2).

| Shim type | Standard type | Macro | On g++-14 `-std=c++26` |
|-----------|---------------|-------|------------------------|
| `gof::overloaded` | *(none exists)* | — | **always shim-provided** (used by C8 Composite) |
| `gof::function_ref<Sig>` | `std::function_ref` | `__cpp_lib_function_ref` | **fallback active** (not used by this group) |
| `gof::polymorphic<T>` | `std::polymorphic` | `__cpp_lib_polymorphic` | **fallback active** — used by C7 Bridge and C9 Decorator (deep-clone of the dynamic type via a captured copier, no virtual `clone()`) |
| `gof::indirect<T>` | `std::indirect` | `__cpp_lib_indirect` | **fallback active** (not used by this group) |

**Active shims in this project:** `gof::overloaded` (C8) and the fallback
`gof::polymorphic` (C7, C9) are both active on g++-14. `std::expected`,
`std::shared_ptr`, `std::call_once`, and `std::variant` are standard and used
directly.

## Build & run

```bash
cmake -S . -B build -DCMAKE_CXX_COMPILER=g++-14 -DPATTERN_WERROR=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/structural26_demo
```

> **Toolchain note:** this project requires C++26 facilities (`std::expected`
> monadic ops, *deducing this*) available on **g++ ≥ 14**. The environment's
> default `g++` is 13.3 — configure with `-DCMAKE_CXX_COMPILER=g++-14`. CMake
> **≥ 3.30** is required for C++26 with GCC.
