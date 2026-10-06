/// \file reflect.hpp
/// \brief Reflection showcase (C22) — a generic `to_string(const Aggregate&)`
///        and field enumerator over an *arbitrary* aggregate struct, built with
///        **P2996 static reflection** (`^^T`, `[: :]`, `template for`).
///
/// \warning This translation unit is **build-gated**. P2996 reflection compiles
///          on **no** compiler available in this repository's toolchain
///          (g++-14 / g++-13 / clang-18 all lack `__cpp_reflection`). The real
///          reflection code below lives inside `#if defined(__cpp_reflection)`,
///          so without an experimental P2996 compiler this header is **empty /
///          harmless** — it declares no symbols and cannot break a build. See
///          DESIGN §C26.5 and SRS FR-C26-8 for the gating rationale.
///
/// The idiom demonstrated: instead of hand-writing a per-type `to_string`
/// (a switch or a visitor overload set per struct), a **single** generic
/// function reflects over the aggregate's non-static data members at compile
/// time, splicing each member to read its identifier and value. Adding a field
/// to the struct requires **no** change here.

#ifndef MCPP_REFLECTION26_REFLECT_HPP
#define MCPP_REFLECTION26_REFLECT_HPP

// `__cpp_reflection` is the standard feature-test macro for P2996 static
// reflection. <version> surfaces library feature-test macros; the language
// macro `__cpp_reflection` is predefined by a reflection-capable compiler.
#include <version>

#if defined(__cpp_reflection)

// These headers only exist on an experimental P2996 reflection compiler.
#include <meta>  // std::meta::* — reflection metafunctions (P2996)

#include <sstream>
#include <string>
#include <type_traits>

namespace mcpp::reflection26 {

/// Concept satisfied by plain aggregate class types (the showcase target).
/// Reflection is applied only to aggregates so that enumerating non-static data
/// members is well-defined and field-by-field reconstruction is meaningful.
template <class T>
concept Aggregate = std::is_aggregate_v<T> && std::is_class_v<T>;

/// Render an *arbitrary* aggregate as `TypeName{ field0=v0, field1=v1, ... }`
/// by reflecting over its non-static data members.
///
/// \tparam T  Any aggregate class type.
/// \param  value  The instance whose fields are stringified.
/// \return A human-readable, field-labelled string.
///
/// The body is the whole point of the showcase: one generic implementation
/// replaces N hand-written per-type stringifiers.
template <Aggregate T>
std::string to_string(const T& value) {
    std::ostringstream out;

    // `^^T` is the reflection operator: it yields a std::meta::info reflecting
    // the type T. `identifier_of` reads its source-level name.
    out << std::meta::identifier_of(^^T) << "{ ";

    bool first = true;

    // `template for` iterates a compile-time range of reflections. Each
    // `member` is a `constexpr std::meta::info` reflecting one non-static data
    // member of T, in declaration order.
    template for (constexpr auto member :
                  std::meta::nonstatic_data_members_of(
                      ^^T, std::meta::access_context::current())) {
        if (!first) {
            out << ", ";
        }
        first = false;

        // `identifier_of(member)` yields the field's source name; the splice
        // `value.[: member :]` names the corresponding subobject of `value`,
        // reading the field's actual value.
        out << std::meta::identifier_of(member) << '=' << value.[:member:];
    }

    out << " }";
    return out.str();
}

/// Count the non-static data members of an aggregate type `T` at compile time.
///
/// \tparam T  Any aggregate class type.
/// \return The number of reflected non-static data members.
template <Aggregate T>
consteval std::size_t field_count() {
    std::size_t n = 0;
    template for (constexpr auto member :
                  std::meta::nonstatic_data_members_of(
                      ^^T, std::meta::access_context::current())) {
        // `member` is unused except to count; refer to it to avoid a warning.
        (void)member;
        ++n;
    }
    return n;
}

/// Invoke `fn(name, fieldValue)` once per non-static data member of `value`.
///
/// This is the reusable enumerator behind `to_string`: callers supply any
/// visitor to drive serialization, logging, diffing, etc., without writing a
/// per-type traversal. `fn` is a generic callable (its second parameter type
/// differs per field).
///
/// \tparam T   Any aggregate class type.
/// \tparam Fn  A callable accepting `(std::string_view name, const Field&)`.
template <Aggregate T, class Fn>
void for_each_field(const T& value, Fn&& fn) {
    template for (constexpr auto member :
                  std::meta::nonstatic_data_members_of(
                      ^^T, std::meta::access_context::current())) {
        fn(std::meta::identifier_of(member), value.[:member:]);
    }
}

}  // namespace mcpp::reflection26

#endif  // defined(__cpp_reflection)

#endif  // MCPP_REFLECTION26_REFLECT_HPP
