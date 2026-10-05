# Builder — Creational — C++23

**Pattern:** Builder (GoF creational).
**Standard:** C++23.
**Showcased idioms:** **deducing this** (explicit object parameter) fluent
interface, `std::expected<T, Error>` validated `build()`.

## Problem

Assemble an immutable `HttpRequest` (method, url, headers, body) through a
readable fluent chain, with validation that reports *why* a build failed.

## Design

- Each setter is `template <class Self> auto&& name(this Self&& self, ...)` and
  returns `std::forward<Self>(self)`, so chaining **preserves the caller's value
  category**: chaining on an rvalue yields rvalues (enabling move-out), on an
  lvalue yields lvalue references.
- `build()` is provided as both a `const&` overload (copies fields) and a
  `&&` overload (moves fields out of a temporary). It returns
  `std::expected<HttpRequest, BuildError>`.

### Validation rules

| Condition | Result |
|-----------|--------|
| empty url | `std::unexpected(BuildError::missing_url)` |
| method not in {GET, POST, PUT, DELETE, PATCH} | `invalid_method` |
| any header with empty name | `empty_header_name` |
| otherwise | the assembled `HttpRequest` |

## Build & run

```bash
cmake -S . -B build -DPATTERN_WERROR=ON -DCMAKE_CXX_COMPILER=g++-14
cmake --build build
ctest --test-dir build --output-on-failure
./build/builder_demo
```

> **Toolchain note:** the fluent setters use *deducing this* (P0847), which
> requires **g++ ≥ 14** or **clang ≥ 18**. The default `g++` 13.3 does not
> support it — configure with `-DCMAKE_CXX_COMPILER=g++-14`.
