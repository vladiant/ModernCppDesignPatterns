/// \file adapter_test.cpp
/// \brief Catch2 tests for Adapter (C6): a concept as the target interface; an
///        adapter wraps a legacy Fahrenheit sensor to satisfy it.

#include <mcpp/structural26/adapter.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace mcpp::structural26;

TEST_CASE("Native Celsius source models the concept", "[adapter]") {
    STATIC_REQUIRE(TemperatureSource<CelsiusThermometer>);
    CelsiusThermometer t{"indoor", 21.5};
    REQUIRE(report(t) == "indoor: 21.5 C");
}

TEST_CASE("Legacy sensor does NOT model the concept", "[adapter][edge]") {
    STATIC_REQUIRE_FALSE(TemperatureSource<LegacyFahrenheitSensor>);
}

TEST_CASE("Adapter makes the legacy sensor satisfy the concept", "[adapter]") {
    STATIC_REQUIRE(TemperatureSource<FahrenheitAdapter>);
    FahrenheitAdapter a{LegacyFahrenheitSensor{"outdoor", 32.0}};
    REQUIRE_THAT(a.celsius(),
                 Catch::Matchers::WithinAbs(0.0, 1e-9));
    REQUIRE(a.label() == "outdoor");
}

TEST_CASE("Adapter converts a boiling-point reading", "[adapter]") {
    FahrenheitAdapter a{LegacyFahrenheitSensor{"kettle", 212.0}};
    REQUIRE_THAT(a.celsius(), Catch::Matchers::WithinAbs(100.0, 1e-9));
    REQUIRE(report(a) == "kettle: 100.0 C");
}

TEST_CASE("Adapter handles a negative reading", "[adapter][edge]") {
    FahrenheitAdapter a{LegacyFahrenheitSensor{"freezer", -4.0}};
    REQUIRE_THAT(a.celsius(), Catch::Matchers::WithinAbs(-20.0, 1e-9));
}
