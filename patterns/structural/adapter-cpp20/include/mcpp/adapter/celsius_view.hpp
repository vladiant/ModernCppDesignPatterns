#ifndef MCPP_ADAPTER_CELSIUS_VIEW_HPP
#define MCPP_ADAPTER_CELSIUS_VIEW_HPP

/// \file celsius_view.hpp
/// \brief The adapter: wraps an index-based Fahrenheit source as a lazy
///        `std::ranges` view of Celsius values (C++20).

#include <concepts>
#include <cstddef>
#include <ranges>

namespace mcpp::adapter {

/// Describes the shape the adapter can wrap, so the adapter is reusable with
/// any indexed Fahrenheit source (not just `LegacySensor`).
template <class S>
concept IndexedFahrenheitSource = requires(const S& s, std::size_t i) {
    { s.count() } -> std::convertible_to<std::size_t>;
    { s.fahrenheit_at(i) } -> std::convertible_to<double>;
};

/// Converts a Fahrenheit value to Celsius.
[[nodiscard]] constexpr double fahrenheit_to_celsius(double f) {
    return (f - 32.0) * 5.0 / 9.0;
}

/// Adapter: exposes a lazy `std::ranges` view of Celsius values over an
/// `IndexedFahrenheitSource`.
///
/// \note The view holds a non-owning reference to the source; the source must
///       outlive the view and any iterators obtained from it. Because the view
///       is lazy, iterating twice re-reads the source (so mutations to the
///       source between iterations are observed).
template <IndexedFahrenheitSource S>
class CelsiusView {
public:
    explicit CelsiusView(const S& src) : src_(&src) {}

    /// Returns a lazy view: `iota(0..count) | transform(index -> celsius)`.
    [[nodiscard]] auto celsius() const {
        const S* src = src_;
        return std::views::iota(std::size_t{0}, src->count()) |
               std::views::transform([src](std::size_t i) {
                   return fahrenheit_to_celsius(src->fahrenheit_at(i));
               });
    }

private:
    const S* src_;
};

} // namespace mcpp::adapter

#endif // MCPP_ADAPTER_CELSIUS_VIEW_HPP
