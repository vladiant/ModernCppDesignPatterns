/// \file adapter_test.cpp
/// \brief Catch2 tests for the Adapter pattern (legacy sensor -> ranges view).

#include <mcpp/adapter/celsius_view.hpp>
#include <mcpp/adapter/legacy_sensor.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <ranges>
#include <vector>

using namespace mcpp::adapter;
using Catch::Matchers::WithinRel;

TEST_CASE("LegacySensor satisfies the source concept", "[adapter][concept]") {
    STATIC_REQUIRE(IndexedFahrenheitSource<LegacySensor>);
    STATIC_REQUIRE_FALSE(IndexedFahrenheitSource<int>);
}

TEST_CASE("Adapted Celsius values match the conversion", "[adapter][convert]") {
    LegacySensor sensor{{32.0, 212.0, 98.6}};
    CelsiusView view{sensor};

    std::vector<double> got;
    for (double c : view.celsius()) {
        got.push_back(c);
    }

    REQUIRE(got.size() == 3);
    REQUIRE(got[0] == 0.0);
    REQUIRE_THAT(got[1], WithinRel(100.0));
    REQUIRE_THAT(got[2], WithinRel(37.0));
}

TEST_CASE("Adapted view composes with filter and take", "[adapter][compose]") {
    LegacySensor sensor{{14.0, 32.0, 50.0, 68.0, 86.0}}; // -10,0,10,20,30 C
    CelsiusView view{sensor};

    auto above_freezing = [](double c) { return c > 0.0; };
    std::vector<double> got;
    for (double c : view.celsius() | std::views::filter(above_freezing) | std::views::take(2)) {
        got.push_back(c);
    }

    REQUIRE(got.size() == 2);
    REQUIRE_THAT(got[0], WithinRel(10.0));
    REQUIRE_THAT(got[1], WithinRel(20.0));
}

TEST_CASE("The view is lazy and re-reads the source", "[adapter][lazy]") {
    LegacySensor sensor{{32.0}};
    CelsiusView view{sensor};

    auto celsius = view.celsius();
    REQUIRE(std::ranges::distance(celsius) == 1);

    sensor.push_back(212.0); // mutate source after creating the view

    std::vector<double> got;
    for (double c : view.celsius()) {
        got.push_back(c);
    }
    REQUIRE(got.size() == 2); // re-read saw the new element
    REQUIRE_THAT(got.back(), WithinRel(100.0));
}
