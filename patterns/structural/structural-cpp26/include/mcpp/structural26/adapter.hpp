/// \file adapter.hpp
/// \brief Adapter (C6) — the *target* interface is a `concept`, not an abstract
///        base class. An adapter wraps a legacy/incompatible type so it
///        satisfies the concept.

#ifndef MCPP_STRUCTURAL26_ADAPTER_HPP
#define MCPP_STRUCTURAL26_ADAPTER_HPP

#include "compat.hpp"

#include <concepts>
#include <format>
#include <string>

namespace mcpp::structural26 {

/// The **target interface**, expressed as a concept (no abstract base class):
/// any type that exposes `double celsius() const` and
/// `std::string label() const` is a `TemperatureSource`.
template <class T>
concept TemperatureSource = requires(const T& t) {
    { t.celsius() } -> std::convertible_to<double>;
    { t.label() } -> std::convertible_to<std::string>;
};

/// A modern, already-conforming source (reports in Celsius natively).
class CelsiusThermometer {
public:
    CelsiusThermometer(std::string name, double celsius)
        : name_(std::move(name)), celsius_(celsius) {}

    [[nodiscard]] double celsius() const { return celsius_; }
    [[nodiscard]] std::string label() const { return name_; }

private:
    std::string name_;
    double celsius_;
};
static_assert(TemperatureSource<CelsiusThermometer>);

/// An *incompatible* legacy sensor: it only reports in Fahrenheit and names
/// its reading differently. It does **not** satisfy `TemperatureSource`.
class LegacyFahrenheitSensor {
public:
    LegacyFahrenheitSensor(std::string tag, double fahrenheit)
        : tag_(std::move(tag)), fahrenheit_(fahrenheit) {}

    [[nodiscard]] double read_fahrenheit() const { return fahrenheit_; }
    [[nodiscard]] std::string tag() const { return tag_; }

private:
    std::string tag_;
    double fahrenheit_;
};

/// Adapter: wraps a `LegacyFahrenheitSensor` and exposes the member functions
/// the `TemperatureSource` concept requires, converting units on the fly.
class FahrenheitAdapter {
public:
    explicit FahrenheitAdapter(LegacyFahrenheitSensor sensor)
        : sensor_(std::move(sensor)) {}

    [[nodiscard]] double celsius() const {
        return (sensor_.read_fahrenheit() - 32.0) * 5.0 / 9.0;
    }
    [[nodiscard]] std::string label() const { return sensor_.tag(); }

private:
    LegacyFahrenheitSensor sensor_;
};
static_assert(TemperatureSource<FahrenheitAdapter>);

/// Generic client constrained on the concept — accepts anything that models
/// `TemperatureSource`, whether natively conforming or adapted.
template <TemperatureSource Src>
std::string report(const Src& src) {
    // One decimal place is plenty for a human-readable report.
    return std::format("{}: {:.1f} C", src.label(), src.celsius());
}

}  // namespace mcpp::structural26

#endif  // MCPP_STRUCTURAL26_ADAPTER_HPP
