# Modern C++ Design Patterns

[![CI](https://github.com/vladiant/ModernCppDesignPatterns/actions/workflows/ci.yml/badge.svg)](https://github.com/vladiant/ModernCppDesignPatterns/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

A curated portfolio of **8 classic Gang-of-Four (GoF) design patterns**, each
re-expressed with **modern C++20 or C++23 idioms** instead of the C++98-era
textbook form. The point is not exhaustive pattern coverage but **breadth of
idiom coverage**: every pattern is deliberately paired with a standard whose
language and library features let it be expressed in a cleaner, safer, and often
virtual-free way.

Each pattern is delivered as an **independent, standalone CMake project** —
configure, build, run the demo, and test it on its own — while a top-level
aggregate `CMakeLists.txt` wires the eight C++20/C++23 projects together for
convenience and CI, and optionally adds an opt-in **C++26 idiom tier** (see
*Toolchain & requirements*).

## Patterns

| Pattern | GoF category | C++ standard | Showcased idiom(s) | Project |
|---------|--------------|--------------|--------------------|---------|
| Abstract Factory | Creational | C++20 | `concepts` constraining product families, `std::format` | [`patterns/creational/abstract-factory-cpp20`](patterns/creational/abstract-factory-cpp20/README.md) |
| Builder | Creational | C++23 | **deducing this** fluent setters, `std::expected` validated `build()` | [`patterns/creational/builder-cpp23`](patterns/creational/builder-cpp23/README.md) |
| Adapter | Structural | C++20 | `concepts` + `std::ranges`/views to wrap a legacy interface | [`patterns/structural/adapter-cpp20`](patterns/structural/adapter-cpp20/README.md) |
| Decorator | Structural | C++23 | **deducing this** type-preserving composition, **`static operator()`** | [`patterns/structural/decorator-cpp23`](patterns/structural/decorator-cpp23/README.md) |
| Strategy | Behavioral | C++20 | `std::invocable`/`concepts`, `std::span`, compile-time selection | [`patterns/behavioral/strategy-cpp20`](patterns/behavioral/strategy-cpp20/README.md) |
| Observer | Behavioral | C++20 | `operator<=>` for ordering/dedup, `std::ranges`, `std::format` | [`patterns/behavioral/observer-cpp20`](patterns/behavioral/observer-cpp20/README.md) |
| Command | Behavioral | C++23 | `std::expected`, **`static operator()`**, **`if consteval`** | [`patterns/behavioral/command-cpp23`](patterns/behavioral/command-cpp23/README.md) |
| Visitor | Behavioral | C++23 | `std::variant` + `overloaded`, **deducing this** self-recursion | [`patterns/behavioral/visitor-cpp23`](patterns/behavioral/visitor-cpp23/README.md) |

Four projects target **C++20** and four target **C++23**, with coverage across
all three GoF categories.

## Toolchain & requirements

- **CMake** ≥ 3.28
- **Catch2 v3.7.1**, fetched automatically via `FetchContent` — the **first**
  configure needs network access to clone it.
- A Linux toolchain. The project is developed and validated on **Ubuntu 24.04**;
  there is no Windows/macOS support claim.

Compiler support splits by standard:

| Projects | Compilers | Extra requirement |
|----------|-----------|-------------------|
| 4 × **C++20** (Abstract Factory, Adapter, Strategy, Observer) | **g++ 13**, **g++-14**, **clang-18** | CMake ≥ 3.28 |
| 4 × **C++23** (Builder, Decorator, Command, Visitor) | **g++-14** | CMake ≥ 3.28 |
| 3 × **C++26** idiom tier (`creational/structural/behavioral-cpp26`) | **g++-14** `-std=c++26` | **CMake ≥ 3.30** |

The C++23 subset is built with **GCC 14**. Three of these projects (Builder,
Decorator, Visitor) use *deducing this* (P0847), which **requires GCC 14** — the
default `g++` 13.3 cannot build them, and clang-18 + libstdc++ 13 cannot build
the C++23 subset (it hides `std::expected`). The full aggregate build therefore
requires **g++-14**.

The **C++26 idiom tier** is an opt-in set of three projects (one per GoF
category) enabled with `-DPATTERN_ENABLE_CPP26=ON`. It builds **only** with
**g++-14 `-std=c++26`** and needs **CMake ≥ 3.30** (Ubuntu 24.04's stock CMake
3.28 cannot configure it), so it is **off by default** and leaves the existing
C++20/C++23 aggregate unchanged. A fourth, P2996 **reflection showcase**
(`patterns/reflection/reflection-cpp26`) is committed as source but builds on no
currently available compiler; it is double-gated behind
`PATTERN_ENABLE_REFLECTION` (default `OFF`) and is **never** built in CI.

## Build & test

### Full aggregate build (all 8 projects, g++-14)

This is the primary CI gate. It builds every project with warnings-as-errors and
runs the whole test suite (**46 tests**):

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-14 \
  -DPATTERN_WERROR=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### C++20 subset only (portability, e.g. clang-18)

`PATTERN_CXX20_ONLY=ON` drops the four C++23 projects from the aggregate so a
compiler without full C++23 support only ever sees C++20 code:

```bash
cmake -S . -B build \
  -DCMAKE_CXX_COMPILER=clang++-18 \
  -DPATTERN_WERROR=ON \
  -DPATTERN_CXX20_ONLY=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### C++26 idiom tier (opt-in, g++-14 + CMake ≥ 3.30)

`PATTERN_ENABLE_CPP26=ON` adds the three C++26 projects on top of the full
8-project aggregate. This needs **g++-14 `-std=c++26`** and **CMake ≥ 3.30**
(stock 3.28 cannot configure it). The reflection showcase stays off, so the run
builds and tests the 8 base projects plus the **89** C++26 tests:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-14 \
  -DPATTERN_WERROR=ON \
  -DPATTERN_ENABLE_CPP26=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure   # 135 tests (46 base + 89 C++26)
```

### A single pattern, standalone

Each project owns its `project()` and is buildable from its own directory —
Catch2 is fetched locally when the aggregate isn't providing it:

```bash
cd patterns/behavioral/strategy-cpp20
cmake -S . -B build -DPATTERN_WERROR=ON   # add -DCMAKE_CXX_COMPILER=g++-14 for a C++23 project
cmake --build build
ctest --test-dir build --output-on-failure
./build/strategy_demo
```

Each project's demo executable is named `<pattern>_demo` (e.g. `builder_demo`,
`visitor_demo`); see the per-project README for the exact name and usage.

### Build options

| Option | Default | Effect |
|--------|---------|--------|
| `PATTERN_WERROR` | `OFF` | Treat compiler warnings as errors (`-Werror`). CI builds with `ON`. |
| `PATTERN_CXX20_ONLY` | `OFF` | Aggregate build only: include just the four C++20 projects (for toolchains without full C++23 support). Does not change any subproject's sources, standard, or interface. |
| `PATTERN_ENABLE_CPP26` | `OFF` | Aggregate build only: also add the three C++26 projects (requires **g++-14 `-std=c++26`** and **CMake ≥ 3.30**). Orthogonal to `PATTERN_CXX20_ONLY`; OFF leaves the C++20/C++23 aggregate unchanged. |
| `PATTERN_ENABLE_REFLECTION` | `OFF` | Aggregate build only: also add the P2996 reflection showcase (needs an experimental reflection compiler; only honored when `PATTERN_ENABLE_CPP26=ON`). Never set in CI. |

Testing is wired through CTest and enabled by the standard `BUILD_TESTING`
option (`ON` by default).

## Repository layout

```
.
├── CMakeLists.txt              # top-level aggregate: builds all 8, fetches Catch2 once
├── VERSION                     # project version (pre-release)
├── LICENSE                     # MIT
├── CHANGELOG.md
├── patterns/
│   ├── creational/
│   │   ├── abstract-factory-cpp20/
│   │   ├── builder-cpp23/
│   │   └── creational-cpp26/   # C++26 tier (opt-in: PATTERN_ENABLE_CPP26)
│   ├── structural/
│   │   ├── adapter-cpp20/
│   │   ├── decorator-cpp23/
│   │   └── structural-cpp26/   # C++26 tier (opt-in: PATTERN_ENABLE_CPP26)
│   ├── behavioral/
│   │   ├── strategy-cpp20/
│   │   ├── observer-cpp20/
│   │   ├── command-cpp23/
│   │   ├── visitor-cpp23/
│   │   └── behavioral-cpp26/   # C++26 tier (opt-in: PATTERN_ENABLE_CPP26)
│   └── reflection/
│       └── reflection-cpp26/   # P2996 showcase (gated: PATTERN_ENABLE_REFLECTION; never in CI)
└── docs/
    ├── requirements/SRS.md
    ├── design/DESIGN.md
    ├── testing/ModernCppDesignPatterns-test-report.md
    └── status.md
```

Each `patterns/<category>/<pattern>-cpp<std>/` project contains:

- `CMakeLists.txt` — standalone, with its own `project()`
- `include/mcpp/<pattern>/` — the header-only pattern code
- `app/main.cpp` — a runnable demo
- `tests/` — Catch2 v3 tests
- `README.md` — pattern intent, idiom, and how to build/run/test it

## Documentation

- **Requirements:** [`docs/requirements/SRS.md`](docs/requirements/SRS.md)
- **Design:** [`docs/design/DESIGN.md`](docs/design/DESIGN.md)
- **Test report:** [`docs/testing/ModernCppDesignPatterns-test-report.md`](docs/testing/ModernCppDesignPatterns-test-report.md)
- **Project status:** [`docs/status.md`](docs/status.md)
- **Changelog:** [`CHANGELOG.md`](CHANGELOG.md)

## Continuous integration

[`.github/workflows/ci.yml`](.github/workflows/ci.yml) runs on Ubuntu 24.04:

- **`build-gcc14`** (primary gate) — full 8-project aggregate build with
  `-DPATTERN_WERROR=ON` plus the complete CTest suite, in Debug and Release.
- **`build-clang18`** (portability) — the C++20 subset built and tested with
  clang-18 via `-DPATTERN_CXX20_ONLY=ON`, in Debug and Release.
- **`build-cpp26`** (C++26 idiom tier) — full aggregate plus the three C++26
  projects via `-DPATTERN_ENABLE_CPP26=ON`, built and tested with g++-14, in
  Debug and Release. Because this tier needs **CMake ≥ 3.30** (newer than the
  distro's 3.28), the job provisions a pinned CMake from the official Kitware
  apt repository. The reflection showcase is never enabled, so CI stays green.

## License

Released under the [MIT License](LICENSE). © 2026 Vladislav Antonov.
