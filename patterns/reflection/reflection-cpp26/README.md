# Reflection Showcase — C++26 Idiom Tier (C22, **GATED**)

**Project:** `reflection_cpp26` (directory `patterns/reflection/reflection-cpp26/`)
**Pattern item:** C22 — Reflection showcase.
**Standard:** C++26 (`-std=c++26`).
**Requires:** an **experimental P2996 static-reflection compiler**
(`__cpp_reflection`). **No** compiler in this repo's toolchain
(g++-14 / g++-13 / clang-18) provides it.

> ⚠️ **This project is intentionally excluded from the default and CI build.**
> It is committed as **documented, build-gated source** that must never break
> the default or CI build (SRS FR-C26-8, NFR-C26-4, AC-C26-6/7; DESIGN
> §C26.5, DD-C26-8).

## What the showcase does

A **single** generic `to_string(const Aggregate&)` — plus a `field_count<T>()`
and a `for_each_field(value, fn)` enumerator — works over an **arbitrary**
aggregate struct using **P2996 static reflection**:

```cpp
template for (constexpr auto member :
              std::meta::nonstatic_data_members_of(
                  ^^T, std::meta::access_context::current())) {
    out << std::meta::identifier_of(member) << '=' << value.[:member:];
}
```

It uses the reflection operator `^^T`, the splice `[: member :]`, `template for`
iteration, and the `std::meta` metafunctions `nonstatic_data_members_of` /
`identifier_of`. This replaces **hand-written per-type switches / per-type
visitors**: adding a field to a struct requires no change to the stringifier.

The real reflection code lives in
`include/mcpp/reflection26/reflect.hpp` inside `#if defined(__cpp_reflection)`,
so **without** a reflection compiler the header is empty/harmless and the demo
and tests are trivially-valid, green translation units.

## Why it is gated (two-layer gate)

1. **Aggregate layer (the CI guard).** The root `CMakeLists.txt` only
   `add_subdirectory()`s this project when `PATTERN_ENABLE_REFLECTION=ON`
   (default `OFF`). **CI never sets it**, so this project is never configured in
   CI — CI stays green regardless of C22's unbuildability.
2. **Standalone layer (belt-and-braces).** This project's own `CMakeLists.txt`
   probes the compiler with `CheckCXXSourceCompiles(__cpp_reflection)`. When the
   feature is **absent**, it prints a SKIP status and `return()`s — **configuring
   successfully but creating no targets**. So a developer who configures this
   directory directly (without a reflection compiler) gets a graceful no-op
   instead of a hard error.

## How to opt in

- **Via the aggregate build** (needs both the C++26 tier and reflection):
  ```bash
  cmake -S . -B build -DPATTERN_ENABLE_CPP26=ON -DPATTERN_ENABLE_REFLECTION=ON \
        -DCMAKE_CXX_COMPILER=<your-P2996-reflection-compiler>
  cmake --build build
  ```
- **Standalone** (configure this directory with a reflection compiler):
  ```bash
  cd patterns/reflection/reflection-cpp26
  cmake -S . -B build -DCMAKE_CXX_COMPILER=<your-P2996-reflection-compiler>
  cmake --build build
  ctest --test-dir build --output-on-failure
  ```
  On a reflection-capable compiler this builds `reflection26_demo` and
  `reflection26_tests` (Catch2 v3, discovered via `catch_discover_tests`).

## No reflection compiler? (the normal case here)

Configuring standalone on g++-14 (which lacks `__cpp_reflection`) **succeeds**
and builds **nothing**:

```bash
cd patterns/reflection/reflection-cpp26
cmake -S . -B build -DCMAKE_CXX_COMPILER=g++-14
# -- reflection_cpp26: no __cpp_reflection on this compiler — SKIPPING build
#    (source is present; needs an experimental P2996 compiler). CI stays green.
cmake --build build   # nothing to build; succeeds
```

## Layout

```
reflection-cpp26/
├── CMakeLists.txt                         # __cpp_reflection probe + SKIP/return() gate
├── README.md                              # this file
├── include/mcpp/reflection26/reflect.hpp  # showcase (guarded by __cpp_reflection)
├── app/main.cpp                           # narrative demo (guarded; else prints a note)
└── tests/reflect_test.cpp                 # Catch2 v3 (guarded; else a passing placeholder)
```
