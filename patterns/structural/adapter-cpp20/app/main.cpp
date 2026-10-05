/// \file main.cpp
/// \brief Adapter demo: a legacy Fahrenheit sensor flows through std::ranges.

#include <mcpp/adapter/celsius_view.hpp>
#include <mcpp/adapter/legacy_sensor.hpp>

#include <format>
#include <iostream>
#include <ranges>

int main() {
    using namespace mcpp::adapter;

    // Legacy, index-based, Fahrenheit source.
    LegacySensor sensor{{14.0, 32.0, 50.0, 68.0, 86.0, 104.0}};

    CelsiusView view{sensor};

    std::cout << "All Celsius readings: ";
    for (double c : view.celsius()) {
        std::cout << std::format("{:.1f} ", c);
    }
    std::cout << '\n';

    // The adapted view composes with standard range adaptors.
    auto above_freezing = [](double c) { return c > 0.0; };
    std::cout << "First 3 above freezing: ";
    for (double c : view.celsius() | std::views::filter(above_freezing) | std::views::take(3)) {
        std::cout << std::format("{:.1f} ", c);
    }
    std::cout << '\n';

    // The view is lazy: appending to the source is observed on re-iteration.
    sensor.push_back(212.0);
    auto updated = view.celsius();
    std::cout << std::format("After appending 212F, last Celsius = {:.1f}\n",
                             *std::ranges::prev(std::ranges::end(updated)));

    return 0;
}
