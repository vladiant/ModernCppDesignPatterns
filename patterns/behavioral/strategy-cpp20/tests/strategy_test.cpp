/// \file strategy_test.cpp
/// \brief Catch2 tests for the Strategy pattern reducer.

#include <mcpp/strategy/reducer.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <span>
#include <stdexcept>
#include <vector>

using namespace mcpp::strategy;
using Catch::Matchers::WithinRel;

namespace {
const std::vector<double> kData{3.0, 1.0, 4.0, 1.0, 5.0, 9.0, 2.0}; // sum=25, max=9
}

TEST_CASE("Sum strategy returns the total", "[strategy][sum]") {
    REQUIRE_THAT(reduce(std::span<const double>{kData}, Sum{}), WithinRel(25.0));
}

TEST_CASE("Mean strategy returns the average", "[strategy][mean]") {
    REQUIRE_THAT(reduce(std::span<const double>{kData}, Mean{}), WithinRel(25.0 / 7.0));
}

TEST_CASE("Max strategy returns the largest element", "[strategy][max]") {
    REQUIRE_THAT(reduce(std::span<const double>{kData}, Max{}), WithinRel(9.0));
}

TEST_CASE("A lambda satisfies ReduceStrategy and works", "[strategy][lambda]") {
    auto first = [](std::span<const double> xs) { return xs.empty() ? 0.0 : xs.front(); };
    STATIC_REQUIRE(ReduceStrategy<decltype(first)>);
    REQUIRE_THAT(reduce(std::span<const double>{kData}, first), WithinRel(3.0));
}

TEST_CASE("Empty-span policy", "[strategy][edge]") {
    const std::span<const double> empty{};

    SECTION("Sum and Mean return 0.0") {
        REQUIRE(reduce(empty, Sum{}) == 0.0);
        REQUIRE(reduce(empty, Mean{}) == 0.0);
    }

    SECTION("Max throws std::invalid_argument") {
        REQUIRE_THROWS_AS(reduce(empty, Max{}), std::invalid_argument);
    }
}

TEST_CASE("Concept rejects non-strategies", "[strategy][concept]") {
    STATIC_REQUIRE(ReduceStrategy<Sum>);
    STATIC_REQUIRE(ReduceStrategy<Mean>);
    STATIC_REQUIRE(ReduceStrategy<Max>);
    STATIC_REQUIRE_FALSE(ReduceStrategy<int>);
}
