# QA Test Report — Modern C++ Design Patterns

**QA Engineer** · Date: 2026-10-05 · Commit under test: `a266c2b`
**Verdict: PASS — ready for the Release Engineer.**

---

## 1. Scope

Independent verification of 8 standalone GoF pattern projects (4× C++20, 4× C++23)
against `docs/requirements/SRS.md` (AC-1…AC-8) and `docs/design/DESIGN.md`
(per-pattern normative behavior / test assertions).

## 2. Build & Test Matrix

| Configuration | Compiler | Result |
|---|---|---|
| Aggregate, `-DPATTERN_WERROR=ON` | g++-14 (14.2.0) | Configure + build **clean, zero warnings**; **46/46 tests pass** |
| Aggregate, ASan + UBSan | g++-14 | Build clean; **46/46 pass**; all 8 demos run clean (no leaks/UB) |
| Standalone strategy/observer/adapter/abstract-factory (C++20) | **default g++ 13.3** | Build `-Werror` clean + all tests pass (6/5/4/3) |
| Standalone strategy/observer/adapter/abstract-factory (C++20) | clang++ 18 | Build `-Werror` clean + all tests pass |
| Standalone builder (C++23) | g++-14 | Build `-Werror` clean + 7/7 tests pass |

- Threading: none of the 8 patterns use threads/shared mutable state → TSan pass not required.
- All 8 `*_demo` executables exit 0 with sensible, pattern-illustrating output (AC-2).

## 3. Per-project test counts

| Pattern | Std | Tests (orig → final) |
|---|---|---|
| Abstract Factory | C++20 | 3 → 3 |
| Builder | C++23 | 6 → **7** (+1 added) |
| Adapter | C++20 | 4 → 4 |
| Decorator | C++23 | 5 → 5 |
| Strategy | C++20 | 6 → 6 |
| Observer | C++20 | 5 → **6** (+1 added) |
| Command | C++23 | 10 → 10 |
| Visitor | C++23 | 5 → 5 |
| **Total** | | **44 → 46** |

## 4. Idiom traceability (FR-4 / AC-4)

Confirmed each pattern's named modern idiom is present in source and asserted by tests:
concepts + `std::format` (Abstract Factory); deducing-this setters + `std::expected`
`build()` (Builder); `IndexedFahrenheitSource` concept + `iota`/`transform` view (Adapter);
`static operator()` + deducing-this `with()` (Decorator); `ReduceStrategy`/`std::invocable`
+ `std::span` + compile-time selection (Strategy); `operator<=>` + `std::ranges` dispatch
(Observer); `std::expected` + `static operator()` + `if consteval` `safe_div` (Command);
`std::variant` + `overloaded` + deducing-this self-recursive lambda (Visitor).
Standard split is 4 C++20 / 4 C++23 across all three GoF categories (AC-5). Each project
has a README stating pattern + standard + idiom.

## 5. Forbidden-feature check (A-1 / AC-6)

`grep` across `patterns/` for `std::print`, `std::println`, `std::generator`,
`std::mdspan`, `std::flat_map` → **none found**. Output uses `std::format` + streams.

## 6. Coverage gaps found & closed (test-only, no interface change)

1. **Builder — deducing-this *lvalue* branch untested.** Design states setters on an
   lvalue return lvalue refs to the *same* builder; existing tests only exercised the
   rvalue chain. Added `"Lvalue chaining preserves value category (deducing this)"`
   asserting reference identity (`&b.method(...) == &b`), header accumulation across
   statements, and that the const-lvalue `build()` does not consume the builder.
2. **Observer — re-subscribe after unsubscribe untested.** Added
   `"Re-subscribe after unsubscribe succeeds"` confirming dedup state is tied to
   membership (removed identity can be re-registered and fires once).

Both additions build clean under `-Werror` on g++-14 / g++-13.3 / clang-18 and pass.

## 7. Defects

**None.** No functional, build, warning, or sanitizer defects found.

## 8. Notes / constraints (not defects)

- C++23 projects require g++-14 (deducing this / `std::expected`); clang-18 + libstdc++ 13
  cannot build them — a known, documented toolchain constraint, not a code defect.
- Verification used out-of-source dirs (`build-qa/`, `build-asan/`, all gitignored);
  temporary `/tmp` build dirs were removed.
