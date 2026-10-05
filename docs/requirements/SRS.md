# Software Requirements Specification — Modern C++ Design Patterns

**Project:** vladiant/ModernCppDesignPatterns
**Document type:** Lightweight SRS (portfolio project)
**Status:** Draft for System Architect hand-off
**Date:** 2026-10-05
**Author:** Requirements Analyst

---

## 1. Purpose & Scope

### 1.1 Purpose
Build a curated portfolio collection of classic *Gang of Four* (GoF) design
patterns, each re-expressed with **modern C++20 or C++23 idioms** rather than
the C++98-era textbook form. The collection demonstrates, to a reviewer of the
author's portfolio, fluency in contemporary C++ language and library features
and the judgment to apply them to well-known design problems.

### 1.2 Scope
- A representative subset of **8 GoF patterns** spanning all three GoF
  categories (Creational, Structural, Behavioral).
- Each pattern is delivered as an **independent, standalone CMake project**
  that can be configured, built, run, and tested on its own.
- Each pattern targets exactly one C++ standard — **either C++20 or C++23** —
  chosen so the pattern can showcase idioms characteristic of that standard.
  Not every pattern appears in both standards; the point is breadth of idiom
  coverage, not exhaustive pattern coverage.
- Clarity and idiomatic style take priority over feature maximalism.

### 1.3 Non-Goals
See Section 7 (Out of Scope).

---

## 2. Proposed Pattern List

Eight patterns, four targeting **C++20** and four targeting **C++23**, with at
least one pattern per standard in every GoF category wherever practical.

| # | Pattern | Category | Target Std | Showcased modern idiom(s) |
|---|---------|----------|-----------|----------------------------|
| P1 | Abstract Factory | Creational | **C++20** | `concepts` constraining product families; `std::format` for demo output |
| P2 | Builder | Creational | **C++23** | *deducing this* (explicit object parameter) for a fluent interface that preserves value/ref categories; `std::expected<T, Error>` for a validated `build()` |
| P3 | Adapter | Structural | **C++20** | `concepts` + `std::ranges`/views to adapt an incompatible interface or range |
| P4 | Decorator | Structural | **C++23** | *deducing this* for recursive, type-preserving composition; `static operator()` for stateless decorator layers |
| P5 | Strategy | Behavioral | **C++20** | `concepts` / `std::invocable`, `std::span` over input data, compile-time strategy selection |
| P6 | Observer | Behavioral | **C++20** | three-way comparison (`operator<=>`) for subscriber ordering/dedup; `std::ranges` for notification dispatch; `std::format` |
| P7 | Command | Behavioral | **C++23** | `std::expected` for `execute()`/`undo()` results; `static operator()` for stateless commands; `if consteval` for compile-time-validated commands |
| P8 | Visitor | Behavioral | **C++23** | `std::variant` + an `overloaded`/visitor built with *deducing this* for recursive traversal of a composite structure |

**Standard split:** C++20 → P1, P3, P5, P6. C++23 → P2, P4, P7, P8 (4 / 4).

**Category coverage:** Creational (P1, P2), Structural (P3, P4),
Behavioral (P5, P6, P7, P8). Creational and Structural each have one C++20 and
one C++23 example; Behavioral has two of each.

> The exact pattern-to-standard assignments may be refined by the System
> Architect if a cleaner idiom demonstration emerges, provided the 4/4
> standard split and all-category coverage are preserved.

---

## 3. Functional Requirements

- **FR-1 — Standalone projects.** Each of the 8 patterns shall be an
  independent project with its own `CMakeLists.txt` that can be configured and
  built in isolation (no dependency on a monolithic top-level build to produce
  that pattern's artifacts).
- **FR-2 — Runnable demo.** Each pattern shall produce a runnable demo
  executable that exercises the pattern and prints human-readable output
  illustrating the pattern's behavior.
- **FR-3 — Unit tests.** Each pattern shall include automated unit tests that
  verify the pattern's observable behavior/contract and that run via `ctest`.
- **FR-4 — Idiom demonstration.** Each pattern's code shall visibly use the
  modern idiom(s) named for it in Section 2, in a way a reviewer can identify
  (e.g., a `concept` definition, an explicit object parameter signature, a
  `std::expected` return type).
- **FR-5 — Correct standard enforced.** Each project shall compile under and
  require its assigned C++ standard (C++20 or C++23) via its own build
  configuration.
- **FR-6 — Aggregate build (optional convenience).** The repository may provide
  a top-level mechanism to configure/build/test all patterns together for CI
  convenience, but this must not be required to build any single pattern
  (supports FR-1). *(The System Architect decides whether to provide this and
  how — e.g. top-level CMake with `add_subdirectory`, or none.)*
- **FR-7 — Discoverability.** Each pattern directory shall contain a short
  README (or equivalent doc) stating the pattern, its target standard, and the
  idiom demonstrated, so the collection is self-describing for a portfolio
  reviewer.

---

## 4. Non-Functional Requirements

- **NFR-1 — Compiler portability.** Every project shall build cleanly with both
  **g++ 13.3** and **clang++ 18** at its assigned standard. Code must avoid
  compiler-specific extensions.
- **NFR-2 — Minimum toolchain.** CMake **≥ 3.28** is the baseline build system
  version.
- **NFR-3 — Warnings policy.** Projects should enable a strong warning set
  (`-Wall -Wextra -Wpedantic`) and should treat warnings as errors
  (`-Werror`) in a CI/strict configuration. *(Recommendation: make
  warnings-as-errors a toggleable option, default ON for CI, so local
  experimentation stays friction-free. Final mechanism is the Architect's
  call.)*
- **NFR-4 — Standard enforcement.** Each project shall set
  `CMAKE_CXX_STANDARD` to its target (20 or 23) with
  `CMAKE_CXX_STANDARD_REQUIRED ON` and `CMAKE_CXX_EXTENSIONS OFF`.
- **NFR-5 — Test framework.** **Recommendation: Catch2 (v3), fetched via CMake
  `FetchContent`.** Rationale: minimal boilerplate keeps each small standalone
  project lean and the pattern code front-and-center (portfolio clarity goal);
  single dependency; integrates with `ctest` via `catch_discover_tests`; easy,
  self-contained fetch with no system install. *GoogleTest (also FetchContent-
  friendly, more industry-recognized) is an acceptable alternative if the
  stakeholder prefers industry familiarity over brevity.* Whichever is chosen,
  it shall be used **consistently across all 8 projects.**
- **NFR-6 — Layout expectations (high level only).** Each pattern lives in its
  own directory under a patterns root, grouped or named so the GoF category and
  pattern are obvious. Each pattern directory contains its sources, its tests,
  its `CMakeLists.txt`, and its README. *Detailed internal layout, naming
  conventions, and the grouping scheme are left to the System Architect.*
- **NFR-7 — Dependency hygiene.** External dependencies shall be limited to the
  chosen test framework (and the standard library). No pattern shall require a
  system-installed third-party library.
- **NFR-8 — Readability.** Code shall favor clear, idiomatic, well-commented
  style over cleverness, since the primary audience is a human reviewer.

---

## 5. Assumptions & Constraints

- **A-1 — libstdc++ 13 library gaps.** Some bleeding-edge C++23 *library*
  features are **not reliably available in libstdc++ 13** (ships with g++ 13.3),
  notably `std::print`/`std::println`, `std::generator`, `std::mdspan`, and
  `std::flat_map`. **Constraint:** patterns targeting C++23 shall prefer
  **language** features (deducing this / explicit object parameter,
  `if consteval`, `static operator()`, multidimensional `operator[]`) and the
  **widely supported** library features (`std::expected`, ranges additions).
  Avoid the listed unavailable features, or guard them behind feature-test
  macros with a fallback. For formatted output prefer `std::format` + streams
  over `std::print`.
- **A-2 — Toolchain is fixed/verified.** g++ 13.3, clang++ 18, CMake 3.28 are
  installed and confirmed to compile both `-std=c++20` and `-std=c++23`.
- **A-3 — Single-platform baseline.** Primary target is **Linux**. Code should
  avoid platform-specific APIs so it remains portable, but Windows/macOS support
  is not a committed requirement this iteration (see Open Questions).
- **A-4 — Licensing.** Repository is MIT-licensed; all contributed code and any
  fetched dependencies must be license-compatible.
- **A-5 — Scope cap.** The deliverable is fixed at the ~8 patterns in Section 2
  for this iteration; additional patterns are future work.

---

## 6. Open Questions

- **OQ-1** — Is cross-platform build (Windows MSVC / macOS clang) a goal for
  this iteration, or is Linux + gcc/clang sufficient? *(Affects NFR-1 scope.)*
- **OQ-2** — Test framework final choice: Catch2 (recommended) vs GoogleTest
  (more industry-recognized)? Stakeholder preference requested.
- **OQ-3** — Is a top-level aggregate build / CI pipeline (GitHub Actions)
  wanted now, or deferred? *(Affects FR-6.)*
- **OQ-4** — Preferred code-formatting / linting standard (e.g. `clang-format`
  style, `clang-tidy`)? Not required but relevant to portfolio polish.
- **OQ-5** — May the pattern-to-standard mapping in Section 2 be adjusted by the
  Architect if a stronger idiom demonstration is found, keeping the 4/4 split?

---

## 7. Out of Scope

- Patterns beyond the 8 selected (e.g. Singleton, Prototype, Facade, Bridge,
  Flyweight, Proxy, Chain of Responsibility, Mediator, Memento, State,
  Template Method, Iterator, Interpreter) — future iterations.
- Implementing every pattern in both C++20 and C++23.
- Production-grade, reusable library packaging (install targets, `find_package`
  config, vcpkg/Conan recipes, ABI stability).
- Performance benchmarking or optimization work.
- Graphical, networked, or interactive demos — console output is sufficient.
- Use of the libstdc++ 13-unavailable C++23 library features listed in A-1.

---

## 8. Acceptance Criteria

- **AC-1** — All 8 patterns from Section 2 exist, each in its own standalone
  CMake project, and each configures and builds **in isolation** with both
  g++ 13.3 and clang++ 18 at its assigned standard. *(FR-1, FR-5, NFR-1, NFR-4)*
- **AC-2** — Each pattern builds a demo executable that runs to completion with
  exit code 0 and prints output illustrating the pattern. *(FR-2)*
- **AC-3** — Each pattern has unit tests discoverable and passing via `ctest`.
  *(FR-3, NFR-5)*
- **AC-4** — Each pattern's source visibly uses its named modern idiom(s), and
  its README states pattern + target standard + idiom. *(FR-4, FR-7)*
- **AC-5** — The standard split is **4 C++20 / 4 C++23** and all three GoF
  categories are represented. *(Section 2)*
- **AC-6** — No project depends on a C++23 library feature unavailable in
  libstdc++ 13 without a guarded fallback; `std::print` is not used
  unconditionally. *(A-1)*
- **AC-7** — Building with warnings-as-errors enabled produces no warnings on
  either compiler. *(NFR-3)*
- **AC-8** — The only external dependency is the chosen test framework, fetched
  via `FetchContent`; no system-installed third-party libraries required.
  *(NFR-7)*

---

## 9. Hand-off

**Requirements are ready for the System Architect agent to design against.**

**Final pattern list and target standards:**

| Pattern | Category | Standard |
|---------|----------|----------|
| Abstract Factory | Creational | C++20 |
| Builder | Creational | C++23 |
| Adapter | Structural | C++20 |
| Decorator | Structural | C++23 |
| Strategy | Behavioral | C++20 |
| Observer | Behavioral | C++20 |
| Command | Behavioral | C++23 |
| Visitor | Behavioral | C++23 |

**What the System Architect must decide next:**
1. The concrete directory layout and pattern grouping/naming scheme (NFR-6),
   and relocate this SRS to `docs/requirements/SRS.md`.
2. Whether/how to provide an optional top-level aggregate build and CI (FR-6, OQ-3).
3. The exact warnings-as-errors mechanism (toggle option vs always-on) (NFR-3).
4. Test framework confirmation — Catch2 (recommended) vs GoogleTest — and the
   FetchContent integration pattern applied uniformly across projects (NFR-5, OQ-2).
5. Feature-test-macro / fallback strategy for any C++23 library feature with
   shaky libstdc++ 13 support, and the standard output approach
   (`std::format` + streams vs guarded `std::print`) (A-1).
6. Resolution of Open Questions OQ-1, OQ-4, OQ-5 with the stakeholder as needed.
