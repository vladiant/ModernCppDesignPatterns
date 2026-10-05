/// \file main.cpp
/// \brief Strategy pattern demo: pluggable numeric reducers over std::span.

#include <mcpp/strategy/reducer.hpp>

#include <algorithm>
#include <format>
#include <iostream>
#include <span>
#include <vector>

int main() {
    using namespace mcpp::strategy;

    const std::vector<double> data{3.0, 1.0, 4.0, 1.0, 5.0, 9.0, 2.0};
    const std::span<const double> view{data};

    std::cout << std::format("data           = {}\n", [&] {
        std::string s = "[";
        for (std::size_t i = 0; i < data.size(); ++i) {
            s += std::format("{}{}", i ? ", " : "", data[i]);
        }
        return s + "]";
    }());

    // Compile-time strategy selection via template parameter.
    std::cout << std::format("reduce(Sum)    = {}\n", reduce(view, Sum{}));
    std::cout << std::format("reduce(Mean)   = {}\n", reduce(view, Mean{}));
    std::cout << std::format("reduce(Max)    = {}\n", reduce(view, Max{}));

    // A plain lambda is also a valid strategy (satisfies ReduceStrategy).
    auto range_lambda = [](std::span<const double> xs) {
        return Max{}(xs) - *std::min_element(xs.begin(), xs.end());
    };
    static_assert(ReduceStrategy<decltype(range_lambda)>,
                  "a lambda of the right shape must satisfy ReduceStrategy");
    std::cout << std::format("reduce(lambda range) = {}\n", reduce(view, range_lambda));

    // Empty-span policy: Sum/Mean -> 0.0; Max would throw (not shown here).
    const std::span<const double> empty{};
    std::cout << std::format("reduce(Sum, empty)  = {}\n", reduce(empty, Sum{}));
    std::cout << std::format("reduce(Mean, empty) = {}\n", reduce(empty, Mean{}));

    return 0;
}
