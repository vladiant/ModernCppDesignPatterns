#ifndef MCPP_ADAPTER_LEGACY_SENSOR_HPP
#define MCPP_ADAPTER_LEGACY_SENSOR_HPP

/// \file legacy_sensor.hpp
/// \brief The adaptee: an incompatible legacy sensor interface (C++20).
///
/// The legacy API is index-based, reports Fahrenheit, and offers no
/// `begin()`/`end()` — it cannot be used with `std::ranges` directly.

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace mcpp::adapter {

/// Adaptee: incompatible legacy interface (no range API, Fahrenheit, by index).
class LegacySensor {
public:
    LegacySensor() = default;

    /// Construct with a set of Fahrenheit readings.
    explicit LegacySensor(std::vector<double> fahrenheit_readings)
        : readings_(std::move(fahrenheit_readings)) {}

    /// Number of readings available.
    [[nodiscard]] std::size_t count() const { return readings_.size(); }

    /// Reading at index \p i, in Fahrenheit.
    /// \throws std::out_of_range if `i >= count()`.
    [[nodiscard]] double fahrenheit_at(std::size_t i) const {
        if (i >= readings_.size()) {
            throw std::out_of_range{"LegacySensor::fahrenheit_at: index out of range"};
        }
        return readings_[i];
    }

    /// Append a reading (used by the demo/tests to show lazy re-reading).
    void push_back(double fahrenheit) { readings_.push_back(fahrenheit); }

private:
    std::vector<double> readings_;
};

} // namespace mcpp::adapter

#endif // MCPP_ADAPTER_LEGACY_SENSOR_HPP
