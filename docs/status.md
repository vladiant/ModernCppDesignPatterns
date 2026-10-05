# Project Status — Modern C++ Design Patterns

**As of:** 2026-10-05
**Version:** `0.1.0` (released; tag `v0.1.0`)
**State:** Released — feature-complete, QA-passed, documented, first version tagged.

## Summary

All eight standalone GoF pattern projects are implemented, tested, and QA-signed
off. CI is green on both jobs. Repo-facing documentation (README, CHANGELOG,
per-project READMEs, design, SRS, test report) is in place.

## Deliverables

| Item | State |
|------|-------|
| 8 pattern projects (4× C++20, 4× C++23) | Complete |
| Aggregate `CMakeLists.txt` (+ `PATTERN_WERROR`, `PATTERN_CXX20_ONLY`) | Complete |
| Catch2 v3.7.1 test suite (46 tests) | Passing |
| ASan + UBSan | Clean |
| CI (`build-gcc14` primary, `build-clang18` portability) | Green |
| SRS / Design / Test report | Complete |
| Top-level README, CHANGELOG, per-project READMEs | Complete |

## Toolchain facts

- Full aggregate build + the four C++23 projects require **g++-14** (deducing
  this, P0847).
- The four C++20 projects also build on **g++ 13** and **clang-18**.
- Linux / Ubuntu 24.04 baseline; no Windows/macOS support claimed.

## Release history

- **v0.1.0** (2026-10-05) — first release: the initial portfolio of eight
  modern-C++ GoF patterns. `CHANGELOG.md` holds the detailed entry.
