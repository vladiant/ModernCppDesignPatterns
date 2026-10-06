/// \file strategy_test.cpp
/// \brief Catch2 tests for Strategy (C13): per-call function_ref + stored
///        move_only_function pricing.

#include <mcpp/behavioral26/strategy.hpp>

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>
#include <vector>

using namespace mcpp::behavioral26;

TEST_CASE("sort_with applies a per-call ordering strategy", "[strategy]") {
    std::vector<int> v{3, 1, 4, 1, 5, 9, 2, 6};

    sort_with(v, [](int a, int b) { return a < b; });
    REQUIRE(v == std::vector<int>{1, 1, 2, 3, 4, 5, 6, 9});

    sort_with(v, [](int a, int b) { return a > b; });
    REQUIRE(v == std::vector<int>{9, 6, 5, 4, 3, 2, 1, 1});
}

TEST_CASE("join renders a vector as a bracketed list", "[strategy]") {
    REQUIRE(join({}) == "[]");
    REQUIRE(join({42}) == "[42]");
    REQUIRE(join({1, 2, 3}) == "[1, 2, 3]");
}

TEST_CASE("Checkout uses its stored pricing strategy", "[strategy]") {
    Checkout checkout{[](double s) { return s; }};
    REQUIRE(checkout.total(100.0) == 100.0);

    checkout.set_pricing([rate = 0.8](double s) { return s * rate; });
    REQUIRE(checkout.total(100.0) == 80.0);
}

TEST_CASE("Checkout can store a move-only (resource-owning) strategy",
          "[strategy][edge]") {
    auto base = std::make_unique<double>(5.0);
    Checkout checkout{
        [base = std::move(base)](double s) { return s + *base; }};
    REQUIRE(checkout.total(10.0) == 15.0);
}
