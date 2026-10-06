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

---

## 10. Addendum — C++26 Idiom Tier

> This addendum extends the SRS with a **new third standard tier** (C++26)
> layered on top of the existing C++20/C++23 collection. Sections 1–9 remain
> unchanged and authoritative for the original 8-pattern collection. The IDs
> below are namespaced (`-C26-`) so they never collide with or renumber the
> existing requirements.

### 10.1 Purpose & Scope (C++26 tier)

**Purpose.** Add a C++26 idiom tier that re-expresses a broad set of GoF
patterns using **C++26 language and library idioms**, demonstrating to a
portfolio reviewer fluency with the newest standard's facilities and the
judgment to retire older boilerplate (virtual `clone()`, `std::function`
everywhere, double-dispatch `accept()/visit()`, per-command classes, etc.).

**Scope.**
- A catalogue of **21 pattern→idiom re-expressions plus 1 reflection
  showcase** (Section 10.2), each delivered in the **same standalone-project
  form** already established for the C++20/C++23 tiers (own `project()`, header
  code, runnable `_demo`, Catch2 v3 tests, per-project README).
- The tier is organized as a **distinct standard grouping** alongside the
  existing C++20 and C++23 samples — "different samples for different
  standard," consistent with the current repository structure.
- Clarity and idiomatic style continue to take priority over feature
  maximalism (consistent with Section 1.2 and NFR-8).

**Non-Goals.** See Section 10.5.

### 10.2 Pattern → C++26 Idiom Catalogue (Functional Requirements)

- **FR-C26-1 — Catalogue delivery.** The C++26 tier shall deliver one
  standalone project per row of the table below. Each project shall visibly
  use the named C++26 idiom in place of the "Replaces" column's older
  construct, in a way a reviewer can identify.

| # | Pattern | C++26 idiom | Replaces |
|---|---------|-------------|----------|
| C1 | Singleton | `= delete("reason")` | silent/cryptic deleted-copy errors |
| C2 | Factory Method | `std::move_only_function` creators + `std::expected` | `std::function`, null returns, exceptions |
| C3 | Abstract Factory | concepts over "theme" types | virtual factory hierarchy |
| C4 | Builder | deducing this (`this auto&& self`) | duplicated `&`/`&&` overloads |
| C5 | Prototype | `std::polymorphic<T>` (copy = deep clone) | virtual `clone()` |
| C6 | Adapter | concept as the target interface | abstract target base |
| C7 | Bridge | `std::polymorphic<Impl>` member | raw/unique pointer to implementor |
| C8 | Composite | `std::variant` + recursive lambda via deducing this | virtual Component tree |
| C9 | Decorator | layers owned as `std::polymorphic` | manual clone plumbing |
| C10 | Facade | `std::expected::transform` chain | nested error checks |
| C11 | Flyweight | `shared_ptr<const T>` cache | (n/a) |
| C12 | Proxy | `std::call_once` lazy load | hand-rolled flag + mutex |
| C13 | Strategy | `std::function_ref` (per call) + `move_only_function` (stored) | `std::function` everywhere |
| C14 | Observer | `Signal<Args...>` of move-only slots | `std::function` + copies |
| C15 | Command | do/undo closure pairs | one class per command |
| C16 | State | `variant` states × events, one `std::visit` | State base class + subclasses |
| C17 | Visitor/Interpreter | `std::visit` + `overloaded`, `std::indirect` for recursion | `accept()`/`visit()` double dispatch |
| C18 | Template Method | deducing this + `requires` on hooks | virtual hooks / CRTP |
| C19 | Iterator | `std::generator` (internal-iterator fallback) | hand-written iterator classes |
| C20 | Chain of Responsibility | `optional`-returning handlers | linked handler objects |
| C21 | Memento | plain value copy | friend-access snapshot class |
| C22 | Reflection showcase | `^^T`, `[: :]`, `template for` | hand-written switches / per-type visitors |

- **FR-C26-2 — Standalone projects.** Each C26 item shall be an independent
  CMake project with its own `project()` and `CMakeLists.txt`, configurable and
  buildable in isolation (consistent with FR-1).
- **FR-C26-3 — Runnable demo.** Each C26 item shall produce a runnable
  `_demo` executable that exercises the idiom and prints human-readable output
  (consistent with FR-2).
- **FR-C26-4 — Unit tests.** Each C26 item shall include **Catch2 v3** unit
  tests that verify observable behavior/contract and run via `ctest`
  (consistent with FR-3).
- **FR-C26-5 — Idiom demonstration.** Each C26 item's code shall visibly use
  its named C++26 idiom (FR-C26-1) such that a reviewer can identify it (e.g. a
  `= delete("reason")` declaration, a `this auto&& self` parameter, a
  `std::polymorphic<T>` member, a `std::generator<T>` function).
- **FR-C26-6 — Standard enforced.** Each C26 project shall configure and
  require `-std=c++26` (C++ standard 26) via its own build configuration
  (consistent with FR-5, NFR-4).
- **FR-C26-7 — Compatibility shim for missing library facilities.** For the
  idioms that rely on library types **not provided** by the available
  toolchain (`std::function_ref`, `std::polymorphic`, `std::indirect`, and
  `overloaded` where absent), the tier shall provide a small compatibility
  shim selected via **feature-test macros** so that the idiom's *intent* is
  demonstrated with a standards-tracking fallback when the real type is
  unavailable (see A-C26-1/2). The shim shall prefer the standard type when the
  feature-test macro reports it present.
- **FR-C26-8 — Reflection showcase is build-gated.** Item C22 (reflection
  showcase) shall be present as source but **excluded from the default build
  and from CI** behind an explicit opt-in build gate, since no available
  compiler can build it (see A-C26-3). Its README shall document that it
  requires an experimental reflection-capable compiler.
- **FR-C26-9 — Discoverability.** Each C26 project directory shall contain a
  README stating the pattern, the target standard (C++26), the C++26 idiom
  demonstrated, and — where applicable — which shim/fallback is active on the
  baseline toolchain (consistent with FR-7).
- **FR-C26-10 — Structural consistency with existing tiers.** The C++26 tier
  shall be grouped/organized by standard in the same manner as the existing
  C++20 and C++23 samples, so the three tiers read as a consistent,
  self-describing collection.

### 10.3 Non-Functional Requirements (C++26 tier)

- **NFR-C26-1 — Toolchain reality.** The available compilers are **g++ 13.3,
  g++-14 (14.2), and clang-18**. There is **no g++-15 and no clang-19/20**. The
  C++26 tier shall build on **g++-14 with `-std=c++26`**
  (`__cplusplus == 202400`) as its baseline compiler.
- **NFR-C26-2 — Known-available C++26 facilities.** The tier may rely directly,
  without a shim, on facilities g++-14 `-std=c++26` is verified to provide:
  `std::generator`, `std::move_only_function`, `std::expected`, and
  *deducing this*.
- **NFR-C26-3 — Known-unavailable facilities require shims/gating.** g++-14
  `-std=c++26` does **not** provide `std::function_ref`, `std::polymorphic`,
  `std::indirect`, nor P2996 reflection (`^^T`, `[: :]`, `template for`).
  Items depending on the first three shall use the FR-C26-7 shim; the
  reflection item shall be build-gated per FR-C26-8.
- **NFR-C26-4 — CI must stay green.** The default/CI build shall compile, run,
  and test **all non-gated** C26 items cleanly on the baseline toolchain, with
  **no build and no test failures**. The reflection showcase (C22) being
  unbuildable here shall **not** break CI (it is excluded from the default/CI
  build per FR-C26-8).
- **NFR-C26-5 — Warnings policy.** C26 projects shall enable a strong warning
  set (`-Wall -Wextra -Wpedantic`) and support warnings-as-errors (`-Werror`)
  in the strict/CI configuration with zero warnings on the baseline toolchain
  (consistent with NFR-3).
- **NFR-C26-6 — Build system.** CMake **≥ 3.28** remains the baseline
  (consistent with NFR-2); each project sets the C++ standard to 26 with
  `CMAKE_CXX_STANDARD_REQUIRED ON` and `CMAKE_CXX_EXTENSIONS OFF` (consistent
  with NFR-4).
- **NFR-C26-7 — Test framework consistency.** Catch2 v3 (fetched via
  `FetchContent`) shall be used across all C26 items, matching the existing
  tiers (consistent with NFR-5); no system-installed third-party library shall
  be required (consistent with NFR-7).
- **NFR-C26-8 — Platform baseline.** Linux / Ubuntu 24.04 is the committed
  baseline for the C++26 tier. Cross-platform support is not committed this
  iteration (consistent with A-3).
- **NFR-C26-9 — Readability.** Code shall favor clear, idiomatic,
  well-commented style; the shim/fallback boundaries shall be obvious to a
  reviewer (consistent with NFR-8).

### 10.4 Constraints & Assumptions (C++26 tier)

- **A-C26-1 — Baseline compiler is g++-14.** The C++26 tier is validated
  against g++-14 `-std=c++26`; g++ 13.3 and clang-18 do not implement enough of
  C++26 to serve as baseline for this tier and are **not** required to build
  C26 items. (Contrast with NFR-1, which governs the C++20/C++23 tiers only.)
- **A-C26-2 — Shim is a demonstration aid, not a library.** The FR-C26-7
  compatibility shim exists so the idiom's intent compiles and runs on the
  baseline toolchain today; it is not a production-grade reimplementation of the
  standard types and shall yield to the real standard type as soon as the
  feature-test macro indicates availability.
- **A-C26-3 — Reflection genuinely cannot build here.** P2996 reflection
  (`^^T`, `[: :]`, `template for`) is unavailable on **every** compiler on this
  machine. Item C22 is therefore an **optional, build-gated showcase**: source
  present, excluded from default and CI builds, documented as requiring an
  experimental reflection compiler. It must never be allowed to break CI.
- **A-C26-4 — Toolchain is fixed/verified.** The compiler set and the
  provided/missing feature list in NFR-C26-1..3 are verified facts on the
  development machine at the time of writing and bound the design.
- **A-C26-5 — Licensing.** All C26 code and any fetched dependency remain MIT
  license-compatible (consistent with A-4).
- **A-C26-6 — Scope cap.** The deliverable for this iteration is fixed at the
  catalogue in Section 10.2 (21 buildable items + 1 gated showcase).

### 10.5 Out of Scope (C++26 tier)

- Any pattern/idiom not listed in Section 10.2.
- Making the reflection showcase (C22) part of the default or CI build, or
  providing a non-reflection emulation of it.
- Backporting C++26 items to build under g++ 13.3 or clang-18.
- Production-grade reimplementations of `std::function_ref`,
  `std::polymorphic`, or `std::indirect` (the shim is a demonstration aid only,
  per A-C26-2).
- Cross-platform (Windows/macOS) builds of the C++26 tier this iteration.
- Performance benchmarking; reusable library packaging/install targets.
- Graphical, networked, or interactive demos — console output is sufficient.

### 10.6 Open Questions (C++26 tier)

- **OQ-C26-1** — Exact opt-in mechanism and naming for the reflection build
  gate (e.g. a CMake option) is left to the Architect; requirement only mandates
  that it is off by default and excluded from CI (FR-C26-8).
- **OQ-C26-2** — Should the shim be a single shared compatibility header reused
  across C26 items, or per-project? (Structural decision for the Architect; the
  requirement is only that each item builds standalone — FR-C26-2.)
- **OQ-C26-3** — Should the three standard tiers (C++20/23/26) share one
  top-level aggregate/CI entry point, or keep separate ones? (Relates to FR-6 /
  OQ-3.)

### 10.7 Acceptance Criteria (C++26 tier)

- **AC-C26-1** — All 21 non-gated C26 items (C1–C21) exist, each as its own
  standalone CMake project, and each configures and builds **in isolation** with
  **g++-14 `-std=c++26`**. *(FR-C26-1, FR-C26-2, FR-C26-6, NFR-C26-1)*
- **AC-C26-2** — Each non-gated C26 item builds a `_demo` that runs to
  completion with exit code 0 and prints output illustrating the idiom.
  *(FR-C26-3)*
- **AC-C26-3** — Each non-gated C26 item has Catch2 v3 tests discoverable and
  passing via `ctest`. *(FR-C26-4, NFR-C26-7)*
- **AC-C26-4** — Each C26 item's source visibly uses its named C++26 idiom, and
  its README states pattern + C++26 + idiom (+ active shim where applicable).
  *(FR-C26-5, FR-C26-9)*
- **AC-C26-5** — Items depending on `std::function_ref`, `std::polymorphic`, or
  `std::indirect` build on the baseline toolchain via the feature-test-macro
  shim, and prefer the real standard type when present. *(FR-C26-7, NFR-C26-3)*
- **AC-C26-6** — The reflection showcase (C22) is present as source, excluded
  from the default and CI builds behind an opt-in gate, and documented as
  requiring an experimental reflection compiler. *(FR-C26-8, A-C26-3)*
- **AC-C26-7** — The default/CI build is **green**: all non-gated C26 items
  build, run, and test cleanly, and C22's unbuildability does not affect CI.
  *(NFR-C26-4)*
- **AC-C26-8** — Building the C26 tier with warnings-as-errors enabled produces
  no warnings on g++-14. *(NFR-C26-5)*
- **AC-C26-9** — The C++26 tier is grouped by standard consistently with the
  existing C++20 and C++23 samples. *(FR-C26-10)*

### 10.8 Hand-off (C++26 tier)

**What the System Architect must decide next for the C++26 tier:**
1. Directory/grouping scheme placing the C++26 tier alongside the existing
   C++20 and C++23 samples (FR-C26-10, consistent with NFR-6).
2. Shim packaging: shared compatibility header vs per-project, and the exact
   feature-test macros guarding `std::function_ref` / `std::polymorphic` /
   `std::indirect` / `overloaded` (FR-C26-7, OQ-C26-2).
3. The opt-in build gate for the reflection showcase C22 and its exclusion from
   default/CI (FR-C26-8, OQ-C26-1).
4. CI wiring that keeps the build green on g++-14 while excluding C22
   (NFR-C26-4), and whether the three tiers share one CI entry point
   (OQ-C26-3).
5. Per-item mapping of idiom to a concrete, reviewer-legible demo scenario
   (FR-C26-5) without regressing the standalone-project contract (FR-C26-2).

**Requirements are ready for the System Architect agent to design against.**
