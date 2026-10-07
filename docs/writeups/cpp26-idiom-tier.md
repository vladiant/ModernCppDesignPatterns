# Re-expressing the Gang of Four in C++26 — and the shim that made it compile

*A reference writeup for the `v0.2.0` C++26 idiom tier of
[ModernCppDesignPatterns](https://github.com/vladiant/ModernCppDesignPatterns).*

> **Scope.** This is an engineering reference, not a tutorial on the GoF patterns
> themselves. It assumes you know what a Decorator or a Visitor *is*, and focuses
> on how C++26 facilities change the way you *write* them — and on the toolchain
> reality of doing that today, on a compiler that implements the standard only
> halfway. Every snippet below is taken verbatim (lightly trimmed for length)
> from the headers under `patterns/*/‑cpp26/`; nothing here is aspirational.

---

## 1. Framing: breadth of idiom, not breadth of patterns

The portfolio's thesis is a single sentence: **take the classic Gang-of-Four
patterns and re-express them with modern C++ idioms instead of the C++98-era
textbook form.** The point was never exhaustive pattern coverage — it was
*idiom* coverage. Each pattern is deliberately paired with a language standard
whose features let it be written more cleanly, more safely, and very often
*without the virtual hierarchy the textbook assumes*.

The repository ships in two bodies of work:

* **Eight standalone C++20/C++23 projects**, one pattern per project (released as
  `v0.1.0`). These demonstrate concepts, `std::ranges`, `operator<=>`,
  *deducing this*, `std::expected`, `std::variant` + `overloaded`, and friends.
* **The C++26 idiom tier** (this writeup, released as `v0.2.0`): an **opt-in**
  set of standalone projects that regroup **21 GoF patterns** across all three
  GoF categories around the newest language and library facilities, plus a
  **build-gated reflection showcase** (a 22nd item).

The C++26 tier is grouped differently from the first eight. Twenty-one patterns
plus a reflection showcase is far too many for one-project-per-pattern without
drowning the repo in near-identical CMake. So the tier is delivered as **four
standalone CMake projects, grouped by GoF category** (design decision
`DD-C26-1`):

| Project | Directory | Patterns | Tests |
|---------|-----------|----------|-------|
| `creational_cpp26` | `patterns/creational/creational-cpp26/` | C1–C5 | 21 |
| `structural_cpp26` | `patterns/structural/structural-cpp26/` | C6–C12 | 33 |
| `behavioral_cpp26` | `patterns/behavioral/behavioral-cpp26/` | C13–C21 | 35 |
| `reflection_cpp26` *(gated)* | `patterns/reflection/reflection-cpp26/` | C22 | — (build-gated) |

Each project still honours the standalone contract of the first eight: its own
`project()`, header-only pattern library, a runnable `<group>26_demo`, Catch2 v3
tests, and a per-project README. What is *new* in this tier — and what makes it
worth writing up — is (a) the specific pattern→idiom replacements, (b) the
`compat.hpp` shim that lets them build on a real compiler, and (c) the honest
gating of the one thing that doesn't build anywhere yet.

---

## 2. The idiom survey

This is the heart of the tier: for each pattern, *what modern facility replaces
the textbook machinery, and what does it retire?* The recurring theme is the
disappearance of virtual hierarchies, `clone()` plumbing, null/exception error
channels, hand-rolled synchronization, and double-dispatch `accept()/visit()`
scaffolding.

### 2.1 Singleton (C1) — `= delete("reason")`

The one-liner idiom. C++26's P2573 lets a deleted function carry a *reason
string*, so an accidental copy produces a diagnostic that tells the caller the
fix instead of a bare "use of deleted function":

```cpp
class AppConfig {
public:
    static AppConfig& instance() {           // Meyers accessor
        static AppConfig cfg;
        return cfg;
    }

    AppConfig(const AppConfig&) =
        MCPP_DELETE_MSG("AppConfig is a process-wide singleton; take a const& "
                        "instead of copying it");
    // ... move + assignment likewise deleted with reasons ...
};
```

Note the macro. g++-14 `-std=c++26` does **not** yet define
`__cpp_deleted_function >= 202403L`, so the reason string is preserved in source
(a reviewer sees the idiom) but degrades to a plain `= delete` until the compiler
catches up:

```cpp
#if defined(__cpp_deleted_function) && __cpp_deleted_function >= 202403L
#define MCPP_DELETE_MSG(msg) delete (msg)
#else
#define MCPP_DELETE_MSG(msg) delete
#endif
```

This is the first appearance of the pattern that dominates the whole tier:
**feature-test-macro-select the real facility, else honest fallback.** More on
that in §3.

### 2.2 Prototype / Bridge / Decorator (C5, C7, C9) — `gof::polymorphic<T>`, copy *is* deep clone

Three different GoF patterns, one idiom. The textbook versions of all three lean
on a virtual `clone()` and manual pointer plumbing. C++26's `std::polymorphic<T>`
is a value type whose **copy constructor deep-clones the owned object, including
a derived dynamic type** — so the pattern logic simply copies a value and the
clone falls out for free, with no `clone()` anywhere in the hierarchy.

Prototype (C5) becomes a registry whose "clone" operation is a plain copy:

```cpp
using Proto = gof::polymorphic<Figure>;   // Figure has NO virtual clone()

std::optional<Proto> clone(const std::string& name) const {
    const auto it = prototypes_.find(name);
    if (it == prototypes_.end()) return std::nullopt;
    return it->second;                     // copy == deep clone of the dynamic type
}
```

Bridge (C7) holds its implementor *by value* — copying the abstraction
deep-copies the implementor, no raw/`unique_ptr` to the impl:

```cpp
class Window {
public:
    template <class R>
    Window(std::string title, R renderer)
        : title_(std::move(title)),
          renderer_(gof::polymorphic<Renderer>(std::move(renderer))) {}
    // ...
private:
    gof::polymorphic<Renderer> renderer_;   // value-semantic bridge
};
```

Decorator (C9) owns each wrapped layer as a `gof::polymorphic<Notifier>`, so
copying a decorated stack deep-clones the *whole chain* with no manual clone
plumbing at any layer. **Retires:** virtual `clone()`, the "who owns the inner
pointer" question, and the deep-copy boilerplate that normally rides along with
polymorphic ownership.

### 2.3 Composite / State / Visitor-Interpreter (C8, C16, C17) — `std::variant` + `std::visit` + `overloaded`

A closed set of node/state/expression types is a `std::variant`; the operation is
a `std::visit` over a `gof::overloaded` lambda set. There is **no abstract
`Component` base, no `accept()/visit()` double dispatch.**

Composite (C8) models a filesystem tree as a `std::variant<File, Directory>` and
recurses with a *deducing this* self-recursive lambda — no `std::function`, no
named recursive free function:

```cpp
inline std::size_t total_size(const Node& root) {
    auto sum = [](this auto&& self, const Node& n) -> std::size_t {
        return std::visit(gof::overloaded{
            [](const File& f) -> std::size_t { return f.size; },
            [&self](const Directory& d) -> std::size_t {
                std::size_t acc = 0;
                for (const Node& child : d.children) acc += self(child);
                return acc;
            }}, n);
    };
    return sum(root);
}
```

State (C16) collapses the entire transition table into a *single* `std::visit`
over `(state, event)` — states and events are plain structs in two variants:

```cpp
inline PlayerState transition(const PlayerState& s, const PlayerEvent& e) {
    return std::visit(gof::overloaded{
        [](const Idle&, const Play&)      -> PlayerState { return Playing{1}; },
        [](const Playing& p, const Pause&)-> PlayerState { return Paused{p.track}; },
        [](const Paused& p, const Play&)  -> PlayerState { return Playing{p.track}; },
        [](const auto&, const Stop&)      -> PlayerState { return Idle{}; },
        [](const auto& st, const auto&)   -> PlayerState { return st; },
    }, s, e);
}
```

Visitor/Interpreter (C17) makes `std::visit` *be* the visitor: a new operation is
just a new overload set, added without touching the node types and without an
`accept()` method anywhere. Recursion in the AST needs value semantics for a
recursive type, which is where `gof::indirect<Expr>` comes in (§3):

```cpp
struct BinOp { char op; gof::indirect<Expr> lhs, rhs; };
struct Expr  { std::variant<Num, BinOp> node; };

inline double eval(const Expr& e) {
    return std::visit(gof::overloaded{
        [](const Num& n) { return n.value; },
        [](const BinOp& b) {
            const double l = eval(*b.lhs), r = eval(*b.rhs);
            switch (b.op) { case '+': return l + r; /* ... */ default: return l / r; }
        }}, e.node);
}
```

**Retires:** the `Component`/`State`/`Expr` virtual base, the `accept(Visitor&)`
double-dispatch dance, and the "add an operation = edit every subclass" tax.

### 2.4 Strategy / Observer / Command / Chain (C13, C14, C15, C20) — the function wrappers

Four behavioral patterns are, at heart, "store or pass a callable". C++26 gives
precise tools for *which kind* of callable:

* **Strategy (C13)** uses two, by intent. A *per-call* comparator is a non-owning
  `gof::function_ref` (zero allocation, no ownership); a *stored* pricing
  strategy is a `std::move_only_function` so it may own captured resources:

  ```cpp
  inline void sort_with(std::vector<int>& v,
                        gof::function_ref<bool(int, int)> order) {
      std::ranges::sort(v, order);
  }

  class Checkout {
      using Pricing = std::move_only_function<double(double) const>;
      // ...
  };
  ```

* **Observer (C14)** is a tiny `Signal<Args...>` whose slots are
  `std::move_only_function<void(Args...)>` — a slot may own a `unique_ptr`,
  which a `std::function`-based signal could not hold.

* **Command (C15)** stores do/undo pairs as `std::move_only_function<void()>`
  closures; `History` just moves commands between a done-stack and an
  undone-stack. No `Command` interface, no concrete command-per-action classes.

* **Chain of Responsibility (C20)** is a vector of
  `std::move_only_function<std::optional<std::string>(const Request&) const>`;
  the first handler to return a non-`nullopt` wins.

**Retires:** the `Strategy`/`Observer`/`Command`/`Handler` interface hierarchies
and their concrete-subclass-per-case explosion; and, by using
`move_only_function`, the artificial "callables must be copyable" constraint that
`std::function` imposes.

### 2.5 Builder / Template Method (C4, C18) — *deducing this*

*Deducing this* (P0847) lets one function body serve both `&` and `&&` call forms
and dispatch to a derived type without CRTP or virtuals.

Builder (C4): a single setter body preserves the caller's value category, so an
rvalue chain can move its fields straight into the product, and `build()` returns
a validated `std::expected`:

```cpp
template <class Self>
auto&& with_url(this Self&& self, std::string url) {
    self.url_ = std::move(url);
    return std::forward<Self>(self);
}

[[nodiscard]] std::expected<HttpRequest, BuildError> build(this HttpRequestBuilder&& self) {
    if (const auto err = self.validate()) return std::unexpected(*err);
    return HttpRequest{std::move(self.method_), std::move(self.url_), /* ... */};
}
```

Template Method (C18): the skeleton lives in the base, dispatches to the derived
type's hooks through the explicit object parameter, and a `requires` clause makes
the hook contract part of the signature — **no virtuals, no CRTP**:

```cpp
class Exporter {
public:
    void run(this auto&& self)
        requires requires { self.open(); self.write_rows(); self.close(); }
    {
        self.open();
        self.write_rows();
        self.close();
    }
};
struct CsvExporter  : Exporter { /* open / write_rows / close */ };
struct JsonExporter : Exporter { /* open / write_rows / close */ };
```

A subtlety worth recording (surfaced by QA as `D-2`): on g++-14 a bare
`static_assert(!requires(T x){ x.run(); })` over this constrained `run()` is a
*hard error* rather than yielding `false`. Wrapping the probe in a named concept
restores the SFINAE context — so the negative-branch test uses a
`Runnable<T>` concept, not an inline `requires`.

### 2.6 Adapter / Abstract Factory (C6, C3) — the concept *is* the interface

The "target interface" of an Adapter, and the "abstract factory" contract, need
not be abstract base classes at all — a `concept` expresses the required shape
and is satisfied structurally:

```cpp
template <class T>
concept TemperatureSource = requires(const T& t) {
    { t.celsius() } -> std::convertible_to<double>;
    { t.label() }   -> std::convertible_to<std::string>;
};

class FahrenheitAdapter { /* wraps a legacy sensor, converts units */ };
static_assert(TemperatureSource<FahrenheitAdapter>);

template <TemperatureSource Src>
std::string report(const Src& src);   // generic client, no virtual base in sight
```

**Retires:** the abstract target/factory base class and the vtable it implies; a
mismatch is a compile-time `concept` failure, not a runtime surprise. (Factory
Method, C2, takes the complementary tack: a registry of
`std::move_only_function` creators keyed by name, with `create()` returning
`std::expected<Shape, FactoryError>` — an unknown key is `std::unexpected`, never
null and never a throw.)

### 2.7 Facade (C10) — `std::expected` monadic chaining

A Facade coordinating validate → charge → ship normally becomes a ladder of
`if (error) return;`. With `std::expected`'s monadic `and_then`/`transform`, the
happy path is a flat chain and any step's `std::unexpected` short-circuits the
rest:

```cpp
[[nodiscard]] std::expected<Shipment, OrderError> place(Order order) const {
    return validate(std::move(order))
        .and_then(charge)
        .and_then(ship);
}
```

**Retires:** nested error-check ladders and the temptation to signal subsystem
failures through exceptions or sentinel return values.

### 2.8 Iterator (C19) — `std::generator`

In-order tree traversal becomes a coroutine that *yields* values lazily, instead
of a hand-written stateful iterator or an eager vector:

```cpp
#if defined(__cpp_lib_generator)
inline std::generator<int> inorder(const TreeNode* n) {
    if (!n) co_return;
    co_yield std::ranges::elements_of(inorder(n->left.get()));
    co_yield n->value;
    co_yield std::ranges::elements_of(inorder(n->right.get()));
}
#else
inline void inorder(const TreeNode* n, gof::function_ref<void(int)> visit);
#endif
```

Note the guard: `std::generator` *is* present on g++-14, so the coroutine path is
what actually compiles. The `#else` internal-iterator branch (driven by a
`function_ref`) is kept deliberately (`DD-C26-11`) to keep the idiom honest and
portable — it documents the standards-tracking intent at no cost on the baseline
toolchain.

### 2.9 Flyweight / Proxy / Memento (C11, C12, C21) — standard-library building blocks

Three patterns that need no shim, just the right standard tool:

* **Flyweight (C11)** interns immutable intrinsic state as
  `std::shared_ptr<const Glyph>`; a repeated character returns the *same* shared
  instance. `const` on the pointee means clients cannot mutate shared state.
* **Proxy (C12)** materializes an expensive subject exactly once via
  `std::call_once` + `std::once_flag`, replacing a hand-rolled bool-flag +
  mutex — and the test races 8 threads × 100 first-use calls to prove the
  single-load invariant holds (TSan-clean).
* **Memento (C21)** needs no friend-access snapshot class at all: with value
  semantics the snapshot is just a copy of `(text, cursor)`, and `save()` /
  `restore()` are a return-by-value and an assignment.

**Retires:** hand-written caches with raw ownership, flag+mutex lazy-init
boilerplate, and the friend-coupled Memento snapshot class.

---

## 3. The engineering story: `compat.hpp`, or writing C++ against a half-implemented standard

The idiom survey above is clean because it reads as if the whole C++26 library
existed. It doesn't — not on any compiler you can install today. This is the part
of the tier that is genuinely portfolio-worthy: **writing real, warning-clean,
sanitizer-clean C++26 against a toolchain that implements the standard only
halfway, without lying about what works.**

### 3.1 What g++-14 `-std=c++26` actually gives you

The baseline toolchain is **g++-14 (14.2) with `-std=c++26`** (`__cplusplus ==
202400`). Empirically — and this is documented in the design as verified fact:

| Facility | Present on g++-14 `-std=c++26`? | Action |
|----------|--------------------------------|--------|
| `std::generator` | **yes** | used directly (guarded at call sites) |
| `std::move_only_function` | **yes** | used directly |
| `std::expected` | **yes** | used directly |
| *deducing this* (P0847) | **yes** | used directly |
| `std::function_ref` | **no** | `gof::function_ref` fallback |
| `std::polymorphic` | **no** | `gof::polymorphic` fallback |
| `std::indirect` | **no** | `gof::indirect` fallback |
| P2573 `= delete("reason")` | **no** (needs GCC 15+) | degrade to plain `= delete` |
| P2996 reflection | **no** | project build-gated (§4) |

So four of the headline idioms are real, and three library vocabulary types plus
two language features are missing. The tier has to compile *today* while
remaining honest about which facilities are genuine and which are stand-ins.

### 3.2 The selection idiom: feature-test macro, else honest fallback

`compat.hpp` (namespace `gof`, carried byte-identically by each C++26 project —
`DD-C26-3`) is the single place that resolves this. For every shimmable facility
it uses the **standard feature-test macro** as the gate: if the macro is defined,
alias the real `std::` type; otherwise provide a minimal fallback.

```cpp
#if defined(__cpp_lib_function_ref)
  #include <functional>
  namespace gof { template <class Sig> using function_ref = std::function_ref<Sig>; }
#else
  namespace gof { /* fallback definition */ }
#endif
```

The same shape repeats for `__cpp_lib_polymorphic` and `__cpp_lib_indirect`. Why
feature-test macros rather than `__has_include`? Because a header can exist
without yet *defining* the type; the macro is the standard, precise gate, and it
means the code **automatically upgrades** to the real `std::` type the day g++
ships it — no edit required (`DD-C26-4`). `gof::overloaded` is the one exception:
no standard `overloaded` type exists, so the shim defines it unconditionally.

### 3.3 The fallbacks are honest demonstration aids, not reimplementations

This is the integrity point the writeup exists to make. Each fallback mirrors the
*semantics* of the real type closely enough that **user code is identical whether
the real or the fallback type is active** — but the fallbacks make no claim to be
production-grade, and their limits are documented in the header itself.

`gof::function_ref` is a non-owning `{void*, thunk}` pair with no null state —
exactly mirroring `std::function_ref`'s zero-allocation, non-owning contract.
`gof::polymorphic` is the interesting one: its copier/deleter are *function
pointers instantiated from the concrete `U` at the constructing call site*, so a
copy reconstructs `new U(*static_cast<const U*>(src))` and deep-clones the
**dynamic** type without any virtual `clone()` on `T`:

```cpp
template <class U>
static T* clone_impl(const T* src) { return new U(*static_cast<const U*>(src)); }

polymorphic(const polymorphic& other)
    : ptr_(other.ptr_ ? other.clone_(other.ptr_) : nullptr),
      clone_(other.clone_), destroy_(other.destroy_) {}
```

That is *how* C5/C7/C9's "copy is deep clone" works on the baseline toolchain.
`gof::indirect`, by contrast, is deliberately **non-polymorphic** value semantics
— a single owned heap box with deep value copy — because conflating it with
`polymorphic` would misrepresent both `std::` types (`DD-C26-7`).

The honest limits, stated in the header and the design (SRS `A-C26-2`): the
fallbacks have **no allocator / `memory_resource` support, are not
`constexpr`-usable, and support no incomplete types beyond the construction
site**. The README of every project using them states "fallback `gof::...`
active on g++-14". And the fallbacks are **unit-tested in their own right** —
`polymorphic` is verified to deep-copy a derived type (construct a base-ref from
a derived, copy, mutate the original, assert the clone is unchanged); `indirect`
is verified to deep-copy and to go valueless after a move. If g++ later ships the
real types, the same tests pin the same contract on both paths, catching any
divergence (`R-C26-4`).

The narrative in one line: **this is what it looks like to write against a
standard the toolchain only half-implements and stay honest about it.**

---

## 4. The honestly-gated reflection showcase (C22)

The 22nd item, `reflection-cpp26`, is a P2996 static-reflection showcase: one
generic `to_string(const Aggregate&)` that reflects over an arbitrary aggregate's
non-static data members with `^^T`, the `[: :]` splice, and `template for`,
replacing N hand-written per-type stringifiers:

```cpp
template <Aggregate T>
std::string to_string(const T& value) {
    std::ostringstream out;
    out << std::meta::identifier_of(^^T) << "{ ";
    bool first = true;
    template for (constexpr auto member :
                  std::meta::nonstatic_data_members_of(
                      ^^T, std::meta::access_context::current())) {
        if (!first) out << ", ";
        first = false;
        out << std::meta::identifier_of(member) << '=' << value.[:member:];
    }
    out << " }";
    return out.str();
}
```

The point here is not the code — it's the **integrity of how it ships.** P2996
compiles on **no** compiler in this repo's toolchain (g++-14, g++-13, clang-18
all lack `__cpp_reflection`). Rather than delete the source until a compiler
exists, or pretend it builds, the showcase is committed as documented source and
**double-gated** so it can never break CI:

1. **The whole implementation lives inside `#if defined(__cpp_reflection)`**, so
   on the baseline toolchain the header is empty and harmless — it declares no
   symbols and cannot break a build.
2. **The aggregate never adds the subdirectory** unless
   `PATTERN_ENABLE_REFLECTION` (default `OFF`) is set, and **CI never sets it.**
   For a developer who configures the project standalone, the project's own
   `CMakeLists.txt` additionally probes the compiler for `__cpp_reflection` and,
   if absent, **configures successfully but builds nothing**, printing a clear
   SKIP line.

QA confirmed the gate: configuring `reflection-cpp26` standalone on g++-14 prints
the SKIP message, creates **no** project targets, and `cmake --build` is a no-op.

This is a deliberate choice, not a gap. The idiom is shown truthfully, its
toolchain requirement is stated in the project README, and its unbuildability is
structurally prevented from affecting the green CI of everything else.

---

## 5. Build & CI reality

The tier is **opt-in and leaves the existing build unchanged.** Two orthogonal
aggregate options gate it:

* `PATTERN_ENABLE_CPP26` (default `OFF`) — adds the three buildable C++26
  projects on top of the full 8-project aggregate. Requires **g++-14
  `-std=c++26`** and, critically, **CMake ≥ 3.30**: Ubuntu 24.04's stock CMake
  3.28 cannot configure C++26 on GCC. With the option OFF, the C++20/C++23
  aggregate is behaviorally unchanged.
* `PATTERN_ENABLE_REFLECTION` (default `OFF`) — additionally adds the gated
  reflection showcase; honored only when `PATTERN_ENABLE_CPP26=ON`, and **never
  set in CI.**

A one-command build of the whole thing:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-14 \
  -DPATTERN_WERROR=ON \
  -DPATTERN_ENABLE_CPP26=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure   # 135 tests (46 base + 89 C++26)
```

CI runs on Ubuntu 24.04 with three jobs; the C++26 tier gets its own:

* **`build-gcc14`** (primary gate) — full 8-project aggregate, warnings-as-errors.
* **`build-clang18`** (portability) — the C++20 subset via `PATTERN_CXX20_ONLY=ON`.
* **`build-cpp26`** — full aggregate plus the three C++26 projects via
  `-DPATTERN_ENABLE_CPP26=ON`, in Debug and Release. Because the tier needs CMake
  ≥ 3.30, this job provisions a **pinned CMake 3.30.5** from Kitware's official
  apt repository (rather than trusting the distro package); it is kept as a
  separate job so `build-gcc14` can keep using the distro CMake. Reflection is
  never enabled, so CI stays green.

### Verified numbers (QA sign-off, `v0.2.0`)

* **89 new Catch2 v3 tests** across the three buildable projects (21 + 33 + 35),
  all passing; **135 tests** in the C++26-enabled aggregate (46 base + 89).
* Warning-clean under `-Werror` on g++-14.
* Clean under **AddressSanitizer + UndefinedBehaviorSanitizer** (all three
  projects) and **ThreadSanitizer** (structural-cpp26, which has the only real
  threading — the Proxy's `std::call_once`, raced across 8 threads × 100 calls).
* The reflection showcase verified to configure-and-build-nothing on g++-14.

One environment caveat recorded by QA (`D-1`, not a code defect): on recent
high-entropy-ASLR kernels the TSan binary aborts at test discovery with
`unexpected memory mapping`; running the TSan build under `setarch -R` (reduced
ASLR) restores a clean 33/33.

---

## 6. Takeaways

What the C++26 tier is meant to demonstrate, concretely:

* **Idiom fluency.** Twenty-one GoF patterns re-expressed so that virtual
  hierarchies, `clone()` plumbing, double-dispatch `accept()/visit()`,
  null/exception error channels, and flag+mutex lazy-init largely *disappear* —
  replaced by `std::variant` + `std::visit`, value-semantic `polymorphic`,
  `std::expected` monadic chaining, *deducing this*, the function wrappers, and
  `std::call_once`.
* **Judgment about the toolchain.** Knowing *when* a feature-test shim is the
  right call (library vocabulary types the compiler will ship soon, where a
  minimal fallback preserves identical user code and can auto-upgrade) and *when*
  to gate honestly instead (a language feature no available compiler implements,
  where the only truthful option is committed-but-not-built, structurally
  prevented from touching CI).
* **Honesty as an engineering value.** The fallbacks document their limits and
  are themselves tested; the degradations (P2573 → plain delete, the three
  `gof::` shims, gated reflection) are listed in the READMEs and the QA report as
  expected, non-defect behavior. Nothing claims to work that doesn't.

That last point is the throughline. The tier could have been written to *look*
like pure C++26; instead it's written to *be* real C++26 on a real compiler, with
every gap labelled. That — more than any single idiom — is the thing worth
showing.

---

### Reference map

| Topic | Where to look in the repo |
|-------|---------------------------|
| Design rationale, all `DD-C26-*` decisions | `docs/design/DESIGN.md` §C26 |
| Requirements (FR/NFR-C26-*) | `docs/requirements/SRS.md` §10 |
| The shim | `patterns/*/‑cpp26/include/mcpp/*26/compat.hpp` |
| Per-pattern headers | `patterns/{creational,structural,behavioral}/*-cpp26/include/mcpp/*26/*.hpp` |
| Gated reflection source | `patterns/reflection/reflection-cpp26/include/mcpp/reflection26/reflect.hpp` |
| QA numbers & sanitizer results | `docs/testing/ModernCppDesignPatterns-test-report.md` (C++26 addendum) |
| Build options & CI job | root `CMakeLists.txt`, `.github/workflows/ci.yml` (`build-cpp26`) |
| Changelog entry | `CHANGELOG.md` `[0.2.0]` |
