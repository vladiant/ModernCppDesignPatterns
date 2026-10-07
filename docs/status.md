# Project Status — Modern C++ Design Patterns

**As of:** 2026-10-06
**Version:** `0.2.1` (released; tag `v0.2.1`) — C++26 idiom tier plus its
portfolio writeup, on top of the initial `0.1.0` portfolio.
**State:** Released — C++26 idiom tier implemented, QA-signed-off, wired into the
aggregate build and a new CI job, documented with a long-form writeup, and
tagged `v0.2.1`.

## Summary

The original eight standalone GoF pattern projects (4× C++20, 4× C++23) remain
released under `v0.1.0`. On top of them, a new **opt-in C++26 idiom tier** has
shipped: four standalone projects that regroup 21 GoF patterns around C++26
facilities (three buildable projects) plus a build-gated P2996 reflection
showcase. All three buildable C++26 projects are implemented, tested, and
QA-signed off; CI is green across all three jobs. Repo-facing documentation
(README, CHANGELOG, per-project READMEs, this status) reflects the C++26 tier.

## Deliverables

| Item | State |
|------|-------|
| 8 pattern projects (4× C++20, 4× C++23) | Complete (released v0.1.0) |
| C++26 tier: `creational-cpp26` (C1–C5, 21 tests) | Complete |
| C++26 tier: `structural-cpp26` (C6–C12, 33 tests) | Complete |
| C++26 tier: `behavioral-cpp26` (C13–C21, 35 tests) | Complete |
| C++26 tier: `reflection-cpp26` (C22, P2996 showcase) | Shipped as build-gated source — builds on no available compiler; never in default/CI build |
| Shared `compat.hpp` shim (namespace `gof`) | Complete |
| Aggregate `CMakeLists.txt` (+ `PATTERN_ENABLE_CPP26`, `PATTERN_ENABLE_REFLECTION`) | Complete |
| Catch2 v3.7.1 test suite — 46 base + 89 C++26 = **135** (C++26-enabled aggregate) | Passing |
| ASan + UBSan + TSan (C++26 tier) | Clean |
| CI (`build-gcc14`, `build-clang18`, `build-cpp26`) | Green |
| SRS / Design / Test report | Complete |
| Top-level README, CHANGELOG, per-project READMEs | Complete |

## Toolchain facts

- The original full aggregate build + the four C++23 projects require
  **g++-14** (deducing this, P0847).
- The four C++20 projects also build on **g++ 13** and **clang-18**.
- The **C++26 tier** (`PATTERN_ENABLE_CPP26=ON`) requires **g++-14
  `-std=c++26`** **and CMake ≥ 3.30**; Ubuntu 24.04's stock CMake 3.28 cannot
  configure C++26 on GCC. g++-13 and clang-18 cannot build the C++26 tier.
- On g++-14, `compat.hpp` runs with fallbacks active for `std::function_ref`,
  `std::polymorphic`, and `std::indirect` (not yet provided), while
  `std::generator`, `std::move_only_function`, `std::expected`, and *deducing
  this* are used directly.
- The reflection showcase (`reflection-cpp26`, C22) needs an experimental P2996
  compiler (`__cpp_reflection`); gated behind `PATTERN_ENABLE_REFLECTION`
  (default `OFF`) and never built in CI.
- Linux / Ubuntu 24.04 baseline; no Windows/macOS support claimed.

## CI

- **`build-gcc14`** (primary gate) — full 8-project aggregate, warnings-as-errors,
  Debug + Release.
- **`build-clang18`** (portability) — C++20 subset via `PATTERN_CXX20_ONLY=ON`,
  Debug + Release.
- **`build-cpp26`** (C++26 tier) — full aggregate + the three C++26 projects via
  `PATTERN_ENABLE_CPP26=ON` on g++-14, Debug + Release; provisions a pinned
  **CMake 3.30.5** from Kitware's apt repo. Reflection stays off.

## Release history

- **v0.1.0** (2026-10-05) — first release: the initial portfolio of eight
  modern-C++ GoF patterns. `CHANGELOG.md` holds the detailed entry.
- **v0.2.0** (2026-10-06) — C++26 idiom tier (four projects, 89 new tests,
  `compat.hpp` shim, gated reflection showcase, `PATTERN_ENABLE_CPP26` /
  `PATTERN_ENABLE_REFLECTION` options, `build-cpp26` CI job). `CHANGELOG.md`
  holds the detailed entry.
- **v0.2.1** (2026-10-06) — portfolio writeup for the C++26 idiom tier
  (`docs/writeups/cpp26-idiom-tier.md`); documentation-only.
