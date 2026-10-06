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

---
---

# QA Test Report — Addendum: C++26 Idiom Tier

**QA Engineer** · Date: 2026-10-06 · Commit under test: `a5e8aef`
**Verdict: PASS — ready for the Release Engineer.**

> Clearly-delimited addendum. Does not revise the C++20/C++23 report above;
> it covers only the newly implemented **C++26 idiom tier** (SRS §10, C1–C22).

---

## A1. Scope

Independent verification of the C++26 idiom tier against `docs/requirements/SRS.md`
§10 (FR-C26-1…10, NFR-C26-1…9) and `docs/design/DESIGN.md`. Four standalone
projects:

| Project | SRS items | Builds here? |
|---|---|---|
| `patterns/creational/creational-cpp26` | C1–C5 | yes |
| `patterns/structural/structural-cpp26` | C6–C12 | yes |
| `patterns/behavioral/behavioral-cpp26` | C13–C21 | yes |
| `patterns/reflection/reflection-cpp26` | C22 | **no — build-gated** (verified) |

## A2. Toolchain (exact)

- **Compiler:** g++-14 (GNU 14.2.0), `-std=c++26` (set by each project), configured
  with `-DCMAKE_CXX_COMPILER=g++-14`.
- **CMake:** 4.4.4 (from `$HOME/.local/bin`; satisfies the projects' `>= 3.30`
  requirement — the system `/usr/bin/cmake` 3.28 is too old and was not used).
- **Test framework:** Catch2 v3.7.1 via FetchContent.
- **Platform:** Linux / Ubuntu 24.04 (committed baseline, NFR-C26-8).

## A3. Per-project build / test / sanitizer results

All three buildable projects configured with `-DPATTERN_WERROR=ON` in a clean
`build-qa/`: **configure + build clean, ZERO warnings** (warnings-as-errors
active, so a clean build is proof), **all tests pass**, demo **exits 0** with
sane, pattern-illustrating output. Sanitizer pass rebuilt in `build-asan/` with
`-DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined
-fno-omit-frame-pointer -g"`: **no ASan/UBSan diagnostics** in tests or demos.

| Project | `-Werror` build | ctest (qa) | ASan+UBSan ctest | demo exit |
|---|---|---|---|---|
| creational-cpp26 (C1–C5)  | clean, 0 warn | **21/21** | **21/21**, no diag | 0 |
| structural-cpp26 (C6–C12) | clean, 0 warn | **33/33** | **33/33**, no diag | 0 |
| behavioral-cpp26 (C13–C21)| clean, 0 warn | **35/35** | **35/35**, no diag | 0 |
| **Total** | | **89/89** | **89/89** | all 0 |

**TSan pass (threaded code).** Only Proxy (C12, `std::call_once`) exercises real
threading — `proxy_test.cpp` races 8 threads × 100 first-use calls. A dedicated
`-fsanitize=thread` build of structural-cpp26: **33/33 pass, no data races**
(single-load invariant `load_count() == 1` holds under TSan). See defect D-1 re:
the ASLR workaround required to run it (environment, not code).

## A4. Reflection build-gate confirmation (FR-C26-8 / NFR-C26-4 / AC-C26-6-7)

Configured `patterns/reflection/reflection-cpp26` standalone with g++-14:
- Configure **succeeds** (exit 0) and prints the SKIP line:
  `reflection_cpp26: no __cpp_reflection on this compiler — SKIPPING build … CI stays green.`
- **No project targets created** — `cmake --build` help lists only default CMake
  utility targets (`all/clean/depend/edit_cache/rebuild_cache`); no
  `reflection26_demo` / `reflection26_tests`.
- `cmake --build` is a **no-op** (empty output); **no executables** produced.
- Expected harmless note: `Manually-specified variables were not used by the
  project: PATTERN_WERROR` — the `option()` is defined after the early `return()`,
  so the flag is simply unused on the gated path. Not a defect.

Confirms C22 cannot break CI on the baseline toolchain.

## A5. Idiom traceability (FR-C26-1 / FR-C26-5)

Each non-gated SRS idiom is present in source and exercised by a test:

| # | Pattern | Idiom | Exercised by |
|---|---|---|---|
| C1 | Singleton | `= delete("reason")` (P2573) | `singleton_test` static_asserts on non-copy/move; **degraded** to plain `= delete` on g++-14 (guarded by `__cpp_deleted_function`) |
| C2 | Factory Method | `move_only_function` + `expected` | `factory_method_test` (move-only creators, `unexpected(unknown_kind)`) |
| C3 | Abstract Factory | concepts over themes | `abstract_factory_test` (light/dark families) |
| C4 | Builder | deducing-this setters | `builder_test` (rvalue chain + named-lvalue) |
| C5 | Prototype | `polymorphic<T>` deep clone | `prototype_test` (deep copy, wrapper copy) |
| C6 | Adapter | concept as target | `adapter_test` (concept modelled / not modelled) |
| C7 | Bridge | `polymorphic<Impl>` member | `bridge_test` (deep-copy implementor) |
| C8 | Composite | `variant` + recursive deducing-this lambda | `composite_test` (recursive size/render) |
| C9 | Decorator | layers as `polymorphic` | `decorator_test` (stack order, deep-clone chain) |
| C10 | Facade | `expected::transform` chain | `facade_test` (incl. `transform` success/error) |
| C11 | Flyweight | `shared_ptr<const T>` cache | `flyweight_test` (identity reuse, interning) |
| C12 | Proxy | `std::call_once` lazy load | `proxy_test` (incl. concurrent single-load) |
| C13 | Strategy | `function_ref` (per call) + `move_only_function` (stored) | `strategy_test` (`sort_with`, move-only pricing) |
| C14 | Observer | `Signal<Args...>` move-only slots | `observer_test` (connect/disconnect, move-only slot) |
| C15 | Command | do/undo closure pairs | `command_test` (undo/redo, redo-clear) |
| C16 | State | `variant` states × events + `visit` | `state_test` (full transition narrative) |
| C17 | Visitor/Interpreter | `visit` + `overloaded` + `indirect` | `interpreter_test` (eval, indirect value semantics) |
| C18 | Template Method | deducing-this + `requires` hooks | `template_method_test` (order + **requires-contract, both branches — see A6**) |
| C19 | Iterator | `generator` / internal-iterator | `iterator_test` (in-order, leaf, empty) |
| C20 | Chain of Responsibility | `optional`-returning handlers | `chain_test` (tiers, boundaries, empty chain) |
| C21 | Memento | plain value copy | `memento_test` (restore, independent copies) |
| C22 | Reflection | `^^T` / `[: :]` / `template for` | source + `reflect_test` present; **build-gated** (not compiled on g++-14) — per FR-C26-8 |

`gof::` compat shims for the unavailable `function_ref` / `polymorphic` /
`indirect` (FR-C26-7) are the ones driving C5/C7/C9/C13/C17; their deep-copy and
non-owning semantics are asserted by the tests above.

## A6. Coverage gap found & closed (test-only, no interface change)

**C18 Template Method — negative branch of the `requires`-constrained hook
contract untested.** The existing `template_method_test` asserted only that
complete exporters satisfy `run()`'s `requires` clause (positive path). Added
compile-time coverage of the *rejecting* side via a named concept
`Runnable<T> = requires(T t){ t.run(); }` and four `static_assert`s:
`Runnable<CsvExporter>` / `Runnable<JsonExporter>` hold, while
`!Runnable<PartialExporter>` (missing two hooks) and `!Runnable<Exporter>` (base,
no hooks) confirm the contract SFINAE-rejects incomplete types without a hard
error. Builds clean under `-Werror` (plain + ASan) and the project still reports
35/35; no pattern source changed.

## A7. Defects

**No product defects.** No functional, build, warning, or sanitizer defect in any
C++26 tier source. Two **environment/toolchain** observations (not product
defects), recorded for the Release Engineer:

- **D-1 (Low, environment).** TSan binaries abort at test-discovery with
  `FATAL: ThreadSanitizer: unexpected memory mapping` on this host — the known
  TSan × high-entropy-ASLR incompatibility on recent kernels, **not** a code
  fault. Mitigation: run the TSan build/tests under `setarch -R` (reduced ASLR);
  with that, structural-cpp26 is 33/33 race-free. Recommend the CI TSan job wrap
  with `setarch -R` (or lower `vm.mmap_rnd_bits`).
- **D-2 (Info, toolchain quirk).** On g++-14, a bare
  `static_assert(!requires(T x){ x.run(); })` over the constrained deducing-this
  `run()` is a **hard error** instead of yielding `false`; wrapping the probe in
  a *named concept* restores the SFINAE context. The A6 test uses the concept
  form. Affects test authoring only; no product impact.

**Expected, non-defect degradations** (per task brief / SRS NFR-C26-3), all
guarded or gated — not counted against the tier:
P2573 `= delete("reason")` → plain `= delete` (C1); `std::function_ref` /
`std::polymorphic` / `std::indirect` → `gof::` compat shims (C5/C7/C9/C13/C17);
P2996 reflection → C22 build-gated.

## A8. QA verdict

**PASS — sign-off.** The C++26 idiom tier builds warning-clean under `-Werror` on
g++-14/CMake 4.4.4, all **89/89** tests pass in both plain and ASan+UBSan
configurations, the threaded Proxy is TSan-clean (under `setarch -R`), the
reflection showcase is correctly build-gated to keep CI green, and every SRS
idiom C1–C22 is traceable to a test (C22 gated). No product defects; the two
recorded items are environment/toolchain notes with a stated mitigation.

**Handoff:** Testing complete — ready for the Release Engineer agent to deploy.
Please have CI's TSan job run under `setarch -R` (D-1).
