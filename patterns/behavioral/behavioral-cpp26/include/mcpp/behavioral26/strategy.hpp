/// \file strategy.hpp
/// \brief Strategy (C13) — call-scoped `gof::function_ref` + stored
///        `std::move_only_function` pricing strategy.

#ifndef MCPP_BEHAVIORAL26_STRATEGY_HPP
#define MCPP_BEHAVIORAL26_STRATEGY_HPP

#include "compat.hpp"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace mcpp::behavioral26 {

/// Render an integer vector as `[a, b, c]` for human-readable demo output.
inline std::string join(const std::vector<int>& v) {
    std::string s = "[";
    for (std::size_t i = 0; i < v.size(); ++i) {
        s += (i ? ", " : "") + std::to_string(v[i]);
    }
    return s + "]";
}

/// Sort `v` using a **per-call** ordering strategy. The comparator is passed as
/// a non-owning `gof::function_ref` — zero allocation, no ownership.
inline void sort_with(std::vector<int>& v,
                      gof::function_ref<bool(int, int)> order) {
    std::ranges::sort(v, order);
}

/// Stores a pricing strategy as a move-only callable so a strategy may own
/// resources (e.g. a captured `unique_ptr`).
class Checkout {
public:
    using Pricing = std::move_only_function<double(double) const>;

    explicit Checkout(Pricing p) : pricing_(std::move(p)) {}

    void set_pricing(Pricing p) { pricing_ = std::move(p); }

    double total(double subtotal) const { return pricing_(subtotal); }

private:
    Pricing pricing_;
};

}  // namespace mcpp::behavioral26

#endif  // MCPP_BEHAVIORAL26_STRATEGY_HPP
