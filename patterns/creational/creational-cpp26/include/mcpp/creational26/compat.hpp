/// \file compat.hpp
/// \brief Compatibility shim (namespace `gof`) for C++26 library facilities
///        that are not yet shipped by the baseline toolchain (g++-14
///        `-std=c++26`).
///
/// This header is the **single source of truth** for the C++26 idiom tier's
/// compatibility layer and is copied byte-identically into every C++26 project
/// (DESIGN §C26.3, OQ-C26-2 / DD-C26-3). It selects the **real standard type**
/// when its feature-test macro is defined, otherwise it provides a minimal,
/// honest fallback.
///
/// Facilities provided:
///   - `gof::overloaded`    — always shim-provided (no standard type exists).
///   - `gof::function_ref`  — `std::function_ref` if `__cpp_lib_function_ref`,
///                             else a `{void*, thunk}` non-owning fallback.
///   - `gof::polymorphic`   — `std::polymorphic` if `__cpp_lib_polymorphic`,
///                             else a value type with deep-clone of the dynamic
///                             type via a captured copier.
///   - `gof::indirect`      — `std::indirect` if `__cpp_lib_indirect`, else a
///                             value type owning a heap `T` with deep-copy
///                             value semantics.
///
/// Verified facts on g++-14 `-std=c++26` (DESIGN §C26.3):
///   `std::generator`, `std::move_only_function`, `std::expected`, and
///   *deducing this* are **present** (used directly, no shim). `function_ref`,
///   `polymorphic`, and `indirect` are **absent** → the fallbacks below are
///   active.
///
/// \note The fallbacks are demonstration aids, not production reimplementations
///       (SRS A-C26-2): no allocator / `memory_resource` support, not
///       `constexpr`-usable, no incomplete-type support beyond the construction
///       site.

#ifndef MCPP_GOF_COMPAT_HPP
#define MCPP_GOF_COMPAT_HPP

#include <concepts>
#include <type_traits>
#include <utility>

// ===========================================================================
// gof::overloaded  (always shim-provided — no standard type exists)
// ===========================================================================
//
// Classic aggregate-of-lambdas plus deduction guide. Used as the single-visitor
// argument to std::visit (DESIGN §C26.3.1).
namespace gof {

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

}  // namespace gof

// ===========================================================================
// gof::function_ref<Sig>  (DESIGN §C26.3.2)
// ===========================================================================
//
// Non-owning reference to any callable with signature `Sig`. Trivially copyable,
// no null/empty state, zero allocation per call. Supports both `R(Args...)` and
// `R(Args...) const` signature forms.
#if defined(__cpp_lib_function_ref)

#include <functional>

namespace gof {
template <class Sig>
using function_ref = std::function_ref<Sig>;
}  // namespace gof

#else  // ----------------------------- fallback -----------------------------

namespace gof {

/// Primary template is left undefined; only the call-signature specializations
/// below are usable.
template <class Sig>
class function_ref;

/// Non-const call form: `function_ref<R(Args...)>`.
template <class R, class... Args>
class function_ref<R(Args...)> {
public:
    /// Bind to any callable `f` with a compatible signature.
    /// \pre `f` outlives this `function_ref` (non-owning reference).
    template <class F>
        requires(!std::same_as<std::remove_cvref_t<F>, function_ref>) &&
                std::invocable<F&, Args...>
    function_ref(F&& f) noexcept
        : obj_(const_cast<void*>(
              static_cast<const void*>(std::addressof(f)))),
          thunk_(+[](void* obj, Args... args) -> R {
              using Fn = std::remove_reference_t<F>;
              return static_cast<R>((*static_cast<Fn*>(obj))(
                  std::forward<Args>(args)...));
          }) {}

    function_ref(const function_ref&) noexcept = default;
    function_ref& operator=(const function_ref&) noexcept = default;

    /// Forwards to the referent.
    R operator()(Args... args) const {
        return thunk_(obj_, std::forward<Args>(args)...);
    }

private:
    void* obj_{};
    R (*thunk_)(void*, Args...){};
};

/// Const call form: `function_ref<R(Args...) const>`.
template <class R, class... Args>
class function_ref<R(Args...) const> {
public:
    /// Bind to any callable `f` whose `const` call is compatible.
    /// \pre `f` outlives this `function_ref` (non-owning reference).
    template <class F>
        requires(!std::same_as<std::remove_cvref_t<F>, function_ref>) &&
                std::invocable<const std::remove_reference_t<F>&, Args...>
    function_ref(F&& f) noexcept
        : obj_(const_cast<void*>(
              static_cast<const void*>(std::addressof(f)))),
          thunk_(+[](const void* obj, Args... args) -> R {
              using Fn = std::remove_reference_t<F>;
              return static_cast<R>((*static_cast<const Fn*>(obj))(
                  std::forward<Args>(args)...));
          }) {}

    function_ref(const function_ref&) noexcept = default;
    function_ref& operator=(const function_ref&) noexcept = default;

    R operator()(Args... args) const {
        return thunk_(obj_, std::forward<Args>(args)...);
    }

private:
    const void* obj_{};
    R (*thunk_)(const void*, Args...){};
};

}  // namespace gof

#endif  // __cpp_lib_function_ref

// ===========================================================================
// gof::polymorphic<T>  (DESIGN §C26.3.3)
// ===========================================================================
//
// Value type with deep-copy semantics: copying the wrapper deep-clones the
// owned object, including a derived *dynamic* type, via a copier captured from
// the concrete U at the constructing call site (no virtual clone() required on
// T).
//
// Documented fallback limits (SRS A-C26-2):
//   - Each constructing U must be copy-constructible and complete at the
//     construction site.
//   - U must publicly derive from (or equal) T so `U* -> T*` is valid.
//   - No allocator / memory_resource support, not constexpr-usable, no
//     incomplete-type support beyond the construction site.
#if defined(__cpp_lib_polymorphic)

#include <memory>

namespace gof {
template <class T>
using polymorphic = std::polymorphic<T>;
}  // namespace gof

#else  // ----------------------------- fallback -----------------------------

#include <utility>

namespace gof {

template <class T>
class polymorphic {
public:
    /// Own a concrete `U` (U == T or U publicly derived from T), by value.
    template <class U = T>
        requires(std::derived_from<std::remove_cvref_t<U>, T> ||
                 std::same_as<std::remove_cvref_t<U>, T>)
    explicit polymorphic(U u)
        : ptr_(new std::remove_cvref_t<U>(std::move(u))),
          clone_(&clone_impl<std::remove_cvref_t<U>>),
          destroy_(&destroy_impl<std::remove_cvref_t<U>>) {}

    /// In-place construct a concrete `U` from `args...`.
    template <class U, class... CArgs>
    explicit polymorphic(std::in_place_type_t<U>, CArgs&&... args)
        : ptr_(new U(std::forward<CArgs>(args)...)),
          clone_(&clone_impl<U>),
          destroy_(&destroy_impl<U>) {}

    polymorphic(const polymorphic& other)
        : ptr_(other.ptr_ ? other.clone_(other.ptr_) : nullptr),
          clone_(other.clone_),
          destroy_(other.destroy_) {}

    polymorphic(polymorphic&& other) noexcept
        : ptr_(std::exchange(other.ptr_, nullptr)),
          clone_(std::exchange(other.clone_, nullptr)),
          destroy_(std::exchange(other.destroy_, nullptr)) {}

    polymorphic& operator=(const polymorphic& other) {
        if (this != &other) {
            polymorphic tmp(other);
            swap(tmp);
        }
        return *this;
    }

    polymorphic& operator=(polymorphic&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = std::exchange(other.ptr_, nullptr);
            clone_ = std::exchange(other.clone_, nullptr);
            destroy_ = std::exchange(other.destroy_, nullptr);
        }
        return *this;
    }

    ~polymorphic() { reset(); }

    T& operator*() noexcept { return *ptr_; }
    const T& operator*() const noexcept { return *ptr_; }
    T* operator->() noexcept { return ptr_; }
    const T* operator->() const noexcept { return ptr_; }

    /// True when the wrapper still owns a value (false after a move).
    [[nodiscard]] bool valueless_after_move() const noexcept {
        return ptr_ == nullptr;
    }

private:
    template <class U>
    static T* clone_impl(const T* src) {
        return new U(*static_cast<const U*>(src));
    }
    template <class U>
    static void destroy_impl(T* p) {
        delete static_cast<U*>(p);
    }

    void reset() noexcept {
        if (ptr_ && destroy_) destroy_(ptr_);
        ptr_ = nullptr;
    }

    void swap(polymorphic& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(clone_, other.clone_);
        std::swap(destroy_, other.destroy_);
    }

    T* ptr_{};                 ///< owned, dynamic type may be derived
    T* (*clone_)(const T*){};  ///< copier instantiated knowing concrete U
    void (*destroy_)(T*){};    ///< deleter instantiated knowing concrete U
};

}  // namespace gof

#endif  // __cpp_lib_polymorphic

// ===========================================================================
// gof::indirect<T>  (DESIGN §C26.3.4)
// ===========================================================================
//
// Value type holding a heap `T` with value (non-polymorphic) copy semantics.
// Used for recursive data types (C17 `Expr`).
//   - Copy = deep copy of the single stored T (NOT polymorphic cloning).
//   - Moved-from state is valueless (`ptr_ == nullptr`); dereferencing a
//     moved-from `indirect` is UB (documented).
#if defined(__cpp_lib_indirect)

#include <memory>

namespace gof {
template <class T>
using indirect = std::indirect<T>;
}  // namespace gof

#else  // ----------------------------- fallback -----------------------------

#include <utility>

namespace gof {

template <class T>
class indirect {
public:
    /// Allocate a default-constructed `T`.
    indirect()
        requires std::default_initializable<T>
        : ptr_(new T()) {}

    /// Allocate `T(args...)`.
    template <class... Args>
    explicit indirect(std::in_place_t, Args&&... args)
        : ptr_(new T(std::forward<Args>(args)...)) {}

    indirect(const indirect& other)
        : ptr_(other.ptr_ ? new T(*other.ptr_) : nullptr) {}

    indirect(indirect&& other) noexcept
        : ptr_(std::exchange(other.ptr_, nullptr)) {}

    indirect& operator=(const indirect& other) {
        if (this != &other) {
            indirect tmp(other);
            std::swap(ptr_, tmp.ptr_);
        }
        return *this;
    }

    indirect& operator=(indirect&& other) noexcept {
        if (this != &other) {
            delete ptr_;
            ptr_ = std::exchange(other.ptr_, nullptr);
        }
        return *this;
    }

    ~indirect() { delete ptr_; }

    T& operator*() & noexcept { return *ptr_; }
    const T& operator*() const& noexcept { return *ptr_; }
    T* operator->() noexcept { return ptr_; }
    const T* operator->() const noexcept { return ptr_; }

    /// True when the box is empty (after a move).
    [[nodiscard]] bool valueless_after_move() const noexcept {
        return ptr_ == nullptr;
    }

private:
    T* ptr_{};  ///< sole owned heap box
};

}  // namespace gof

#endif  // __cpp_lib_indirect

#endif  // MCPP_GOF_COMPAT_HPP
