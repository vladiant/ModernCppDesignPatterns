# Changelog

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

Initial content for the first release. No version has been tagged yet; the
project `VERSION` is `0.0.0`.

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

[Unreleased]: https://github.com/vladiant/ModernCppDesignPatterns/commits/main
