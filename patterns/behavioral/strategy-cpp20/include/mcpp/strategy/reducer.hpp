#ifndef MCPP_STRATEGY_REDUCER_HPP
#define MCPP_STRATEGY_REDUCER_HPP

/// \file reducer.hpp
/// \brief Strategy pattern over a numeric reducer (C++20).
///
/// A reducer collapses a `std::span<const double>` to a single `double` using a
/// pluggable strategy. The strategy is a *concept-constrained callable*
/// (`ReduceStrategy`); selection is done at compile time via a template
/// parameter, so there is no virtual dispatch.

#include <concepts>
#include <span>
#include <stdexcept>
#include <type_traits>

namespace mcpp::strategy {

/// A strategy reduces a span of doubles to a double.
/// Any invocable with the shape `double(std::span<const double>)` qualifies,
/// including plain lambdas.
template <class S>
concept ReduceStrategy = std::invocable<S, std::span<const double>> &&
    std::convertible_to<std::invoke_result_t<S, std::span<const double>>, double>;

/// Sum of all elements. Empty span -> 0.0.
struct Sum {
    [[nodiscard]] double operator()(std::span<const double> xs) const {
        double total = 0.0;
        for (double x : xs) {
            total += x;
        }
        return total;
    }
};

/// Arithmetic mean. Empty span -> 0.0 (documented policy).
struct Mean {
    [[nodiscard]] double operator()(std::span<const double> xs) const {
        if (xs.empty()) {
            return 0.0;
        }
        double total = 0.0;
        for (double x : xs) {
            total += x;
        }
        return total / static_cast<double>(xs.size());
    }
};

/// Maximum element. Empty span is a programmer error and throws
/// `std::invalid_argument` (so the return type stays a plain `double`).
struct Max {
    [[nodiscard]] double operator()(std::span<const double> xs) const {
        if (xs.empty()) {
            throw std::invalid_argument{"Max strategy requires a non-empty span"};
        }
        double best = xs.front();
        for (double x : xs.subspan(1)) {
            if (x > best) {
                best = x;
            }
        }
        return best;
    }
};

/// Context: reduce `data` using a compile-time selected strategy `S`.
/// \tparam S a type satisfying `ReduceStrategy`.
template <ReduceStrategy S>
[[nodiscard]] double reduce(std::span<const double> data, S strategy = S{}) {
    return strategy(data);
}

} // namespace mcpp::strategy

#endif // MCPP_STRATEGY_REDUCER_HPP
