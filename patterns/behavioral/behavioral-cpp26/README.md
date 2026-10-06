# Behavioral Patterns — C++26 Idiom Tier

**Project:** `behavioral_cpp26` (directory `patterns/behavioral/behavioral-cpp26/`)
**Patterns:** C13 Strategy · C14 Observer · C15 Command · C16 State ·
C17 Visitor/Interpreter · C18 Template Method · C19 Iterator ·
C20 Chain of Responsibility · C21 Memento.
**Standard:** C++26 (`-std=c++26`), built **only** with **g++-14**.

This is the grouped C++26 behavioral project seeded by the stakeholder reference
translation unit. It also establishes the shared compatibility shim
`compat.hpp` (namespace `gof`) that the other C++26 projects copy byte-for-byte
(DESIGN §C26.3, DD-C26-3).

## Showcased C++26 idioms

| # | Pattern | Visible idiom |
|---|---------|---------------|
| C13 | Strategy | `gof::function_ref<bool(int,int)>` per-call + `std::move_only_function<double(double) const>` stored |
| C14 | Observer | `Signal<Args...>` whose slots are `std::move_only_function` (may own resources) |
| C15 | Command | do/undo pairs as `std::move_only_function<void()>` closures |
| C16 | State | `std::variant` states × events, one `std::visit(gof::overloaded{…})` table |
| C17 | Visitor/Interpreter | `std::visit` + `gof::overloaded`; `gof::indirect<Expr>` for recursion |
| C18 | Template Method | *deducing this* (`run(this auto&& self)`) + `requires` hook contract |
| C19 | Iterator | `std::generator<int>` in-order traversal (present on g++-14) |
| C20 | Chain of Responsibility | `std::optional`-returning `std::move_only_function` handlers |
| C21 | Memento | plain value-copy snapshot (no friend snapshot class) |

## `compat.hpp` — the shared shim (namespace `gof`)

`compat.hpp` selects the real standard type when its feature-test macro is
defined, else provides a minimal honest fallback. The shim is a demonstration
aid, not a production reimplementation (SRS A-C26-2).

| Shim type | Standard type | Macro | On g++-14 `-std=c++26` |
|-----------|---------------|-------|------------------------|
| `gof::overloaded` | *(none exists)* | — | **always shim-provided** |
| `gof::function_ref<Sig>` | `std::function_ref` | `__cpp_lib_function_ref` | **fallback active** (`{void*, thunk}`, both `R(Args...)` and `R(Args...) const` forms) |
| `gof::polymorphic<T>` | `std::polymorphic` | `__cpp_lib_polymorphic` | **fallback active** (deep-clone of dynamic type via captured copier; implemented here for the shared shim — not used by the behavioral group) |
| `gof::indirect<T>` | `std::indirect` | `__cpp_lib_indirect` | **fallback active** (owns heap `T`, deep value copy) |

Facilities used **directly** (present on g++-14, no shim): `std::generator`,
`std::move_only_function`, `std::expected`, and *deducing this*.

### `gof::polymorphic` fallback limits (documented)

- Each constructing `U` must be copy-constructible and complete at the
  construction site.
- `U` must publicly derive from (or equal) `T` so `U* → T*` is valid.
- No allocator / `memory_resource` support, not `constexpr`-usable, no
  incomplete-type support beyond the construction site.

## Build & run

```bash
cd patterns/behavioral/behavioral-cpp26
cmake -S . -B build -DCMAKE_CXX_COMPILER=g++-14 -DPATTERN_WERROR=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/behavioral26_demo
```

> **Toolchain note:** this tier uses `-std=c++26`, `std::generator`,
> `std::move_only_function`, and *deducing this*, which require **g++-14**.
> clang-18 and g++-13 are out of scope for the C++26 tier — configure with
> `-DCMAKE_CXX_COMPILER=g++-14`.
