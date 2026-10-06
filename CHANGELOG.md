# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- **C++26 idiom tier** — a new, opt-in set of standalone projects that regroup
  the GoF patterns around C++26 language and library facilities, following the
  same conventions as the existing C++20/C++23 tiers (own `project()`,
  header-only pattern library, runnable `<project>_demo`, Catch2 v3 tests,
  per-project README). Delivered as **four** projects:
  - **`patterns/creational/creational-cpp26`** (C1–C5): Singleton
    (`= delete("reason")`, guarded to a plain `= delete` on g++-14),
    Factory Method (`std::move_only_function` creators + `std::expected`),
    Abstract Factory (a `concept` over theme types), Builder (*deducing this*
    `this auto&& self` setters + validated `std::expected` `build()`),
    Prototype (`gof::polymorphic<T>` copy-is-deep-clone). **21 tests.**
  - **`patterns/structural/structural-cpp26`** (C6–C12): Adapter (a `concept`
    as the target interface), Bridge (`gof::polymorphic<Impl>` by value),
    Composite (`std::variant` tree + a self-recursive *deducing this* lambda
    over `std::visit`), Decorator (layers as `gof::polymorphic`), Facade
    (`std::expected` monadic `and_then`/`transform` chain), Flyweight
    (`std::shared_ptr<const T>` intern cache), Proxy (`std::call_once`).
    **33 tests.**
  - **`patterns/behavioral/behavioral-cpp26`** (C13–C21): Strategy
    (`gof::function_ref` per-call + `std::move_only_function` stored), Observer
    (`Signal<Args...>` of move-only slots), Command (do/undo
    `std::move_only_function<void()>` closures), State (`std::variant` states ×
    events in one `std::visit`), Visitor/Interpreter (`std::visit` +
    `overloaded` + `gof::indirect`), Template Method (*deducing this* +
    `requires` hooks), Iterator (`std::generator`), Chain of Responsibility
    (`std::optional` handlers), Memento (plain value-copy snapshot).
    **35 tests.**
  - **`patterns/reflection/reflection-cpp26`** (C22): a P2996 static-reflection
    showcase (`^^T`, `[: :]` splice, `template for`, `std::meta`). It is
    **build-gated** — committed as documented source that builds on no compiler
    in this repo's toolchain, so it is excluded from the default and CI builds.
- **Shared `compat.hpp` shim** (namespace `gof`, carried by each C++26 project)
  that selects the real standard facility when its feature-test macro is defined
  and otherwise provides an honest, minimal fallback — because g++-14
  `-std=c++26` provides `std::generator`, `std::move_only_function`,
  `std::expected`, and *deducing this* but **not** `std::function_ref`,
  `std::polymorphic`, `std::indirect`, P2996 reflection, or P2573 delete-reason
  strings. The shim is a demonstration aid, not a production reimplementation.
- **Build options** `PATTERN_ENABLE_CPP26` (default `OFF`; aggregates the three
  buildable C++26 projects on top of the full 8-project aggregate, requires
  **g++-14 `-std=c++26`** and **CMake ≥ 3.30**) and `PATTERN_ENABLE_REFLECTION`
  (default `OFF`; additionally adds the gated reflection showcase, honored only
  when `PATTERN_ENABLE_CPP26=ON`; never set in CI).
- **89 new Catch2 v3 tests** across the three buildable C++26 projects (21 + 33
  + 35), all passing; clean under AddressSanitizer + UndefinedBehaviorSanitizer
  + ThreadSanitizer.
- **Continuous integration**: new `build-cpp26` job
  (`.github/workflows/ci.yml`) that provisions a pinned **CMake 3.30.5** from
  Kitware's apt repository and builds + tests the full aggregate plus the three
  C++26 projects with `-DPATTERN_ENABLE_CPP26=ON` on g++-14, in Debug and
  Release. Reflection is never enabled, so CI stays green.

### Notes

- The C++26 tier requires **CMake ≥ 3.30**; Ubuntu 24.04's stock CMake 3.28
  cannot configure C++26 on GCC. The existing C++20/C++23 aggregate still builds
  with CMake ≥ 3.28, and with `PATTERN_ENABLE_CPP26=OFF` the aggregate is
  behaviorally unchanged.

## [0.1.0] - 2026-10-05

First tagged release: the initial portfolio of eight modern-C++ GoF patterns.

### Added

- **Eight standalone GoF pattern projects**, each a self-contained CMake project
  under `patterns/<category>/<pattern>-cpp<std>/` with a header-only library,
  a runnable `<pattern>_demo`, Catch2 tests, and a per-project README:
  - **Creational** — Abstract Factory (C++20; concepts + `std::format`),
    Builder (C++23; *deducing this* fluent setters + `std::expected` `build()`).
  - **Structural** — Adapter (C++20; concepts + `std::ranges` views),
    Decorator (C++23; *deducing this* composition + `static operator()`).
  - **Behavioral** — Strategy (C++20; `std::invocable`/concepts + `std::span`),
    Observer (C++20; `operator<=>` + `std::ranges`), Command (C++23;
    `std::expected` + `static operator()` + `if consteval`), Visitor (C++23;
    `std::variant` + `overloaded` + *deducing this* recursion).
- **Top-level aggregate build** (`CMakeLists.txt`) that builds all eight
  projects via `add_subdirectory` and fetches Catch2 once for the whole tree.
- **Build options** `PATTERN_WERROR` (treat warnings as errors; default `OFF`,
  CI uses `ON`) and `PATTERN_CXX20_ONLY` (aggregate only the four C++20 projects
  for toolchains without full C++23 support; default `OFF`).
- **Test suite** of 46 Catch2 v3.7.1 tests across all eight projects, wired
  through CTest; clean under AddressSanitizer + UndefinedBehaviorSanitizer.
- **Continuous integration** (`.github/workflows/ci.yml`) on Ubuntu 24.04:
  `build-gcc14` (full aggregate, warnings-as-errors, primary gate) and
  `build-clang18` (C++20 subset portability).
- **Documentation**: requirements (`docs/requirements/SRS.md`), design
  (`docs/design/DESIGN.md`), QA test report
  (`docs/testing/ModernCppDesignPatterns-test-report.md`), top-level `README.md`,
  project status (`docs/status.md`), and this changelog. MIT `LICENSE`.

[Unreleased]: https://github.com/vladiant/ModernCppDesignPatterns/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/vladiant/ModernCppDesignPatterns/releases/tag/v0.1.0
