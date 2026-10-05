#ifndef MCPP_DECORATOR_PIPELINE_HPP
#define MCPP_DECORATOR_PIPELINE_HPP

/// \file pipeline.hpp
/// \brief Decorator pattern as a type-preserving text pipeline (C++23).
///
/// Showcased idioms:
///  - **`static operator()`** for stateless decorator layers (`Uppercase`,
///    `Trim`) — no per-object state, so the call operator is static.
///  - **deducing this** for recursive, type-preserving composition: composing a
///    layer builds a new, fully-typed `Decorated<...>` (no type erasure, no
///    `std::function` in the hot path).

#include <algorithm>
#include <cctype>
#include <concepts>
#include <string>
#include <type_traits>
#include <utility>

namespace mcpp::decorator {

/// A layer is any invocable `std::string -> std::string`.
template <class L>
concept Layer = std::invocable<L, std::string> &&
    std::convertible_to<std::invoke_result_t<L, std::string>, std::string>;

/// Stateless layer: upper-case the whole string. Uses `static operator()`.
struct Uppercase {
    static std::string operator()(std::string s) {
        std::ranges::transform(s, s.begin(),
                               [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
        return s;
    }
};

/// Stateless layer: trim leading/trailing ASCII whitespace. `static operator()`.
struct Trim {
    static std::string operator()(std::string s) {
        auto not_space = [](unsigned char c) { return std::isspace(c) == 0; };
        auto begin = std::ranges::find_if(s, not_space);
        auto end = std::ranges::find_if(s.rbegin(), s.rend(), not_space).base();
        if (begin >= end) {
            return std::string{};
        }
        return std::string{begin, end};
    }
};

/// Stateful layer example: prepends a tag. Holds data, non-static `operator()`.
struct Prefix {
    std::string tag;
    std::string operator()(std::string s) const { return tag + std::move(s); }
};

/// Type-preserving composition via deducing this. Applies `inner_` first, then
/// `layer_`. The composite type grows at compile time
/// (`Decorated<Decorated<...>, Next>`).
template <class Inner, Layer L>
class Decorated {
public:
    Decorated(Inner inner, L layer) : inner_(std::move(inner)), layer_(std::move(layer)) {}

    /// Apply inner first, then this layer.
    std::string operator()(this const Decorated& self, std::string in) {
        return self.layer_(self.inner_(std::move(in)));
    }

    /// Compose another layer on top, preserving the composite's value category.
    template <class Self, Layer Next>
    auto with(this Self&& self, Next next) {
        return Decorated<std::remove_cvref_t<Self>, Next>{std::forward<Self>(self),
                                                          std::move(next)};
    }

private:
    Inner inner_;
    L layer_;
};

/// Identity base: passes text through unchanged. Serves as the pipeline seed.
struct Identity {
    static std::string operator()(std::string s) { return s; }
};

/// Seed a pipeline from a source callable (`std::string(std::string)`-like).
/// Returns a `Decorated` wrapping `src` behind an identity base.
template <Layer Source>
auto decorate(Source src) {
    return Decorated<Identity, Source>{Identity{}, std::move(src)};
}

} // namespace mcpp::decorator

#endif // MCPP_DECORATOR_PIPELINE_HPP
