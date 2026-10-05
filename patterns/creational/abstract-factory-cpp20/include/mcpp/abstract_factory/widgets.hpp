#ifndef MCPP_ABSTRACT_FACTORY_WIDGETS_HPP
#define MCPP_ABSTRACT_FACTORY_WIDGETS_HPP

/// \file widgets.hpp
/// \brief Abstract Factory products + product concepts (C++20).
///
/// The modern twist: the "abstract products" are **concepts**, not base
/// classes. Concrete products are plain value types with a `render()` query.

#include <concepts>
#include <format>
#include <string>

namespace mcpp::abstract_factory {

/// Abstract product: a button renders to text.
template <class T>
concept Button = requires(const T& b) {
    { b.render() } -> std::convertible_to<std::string>;
};

/// Abstract product: a checkbox renders to text.
template <class T>
concept Checkbox = requires(const T& c) {
    { c.render() } -> std::convertible_to<std::string>;
};

// ----- Light family -----

/// Light-theme button.
struct LightButton {
    [[nodiscard]] std::string render() const { return std::format("[ {} ]", "OK"); }
};

/// Light-theme checkbox.
struct LightCheckbox {
    bool checked{};
    [[nodiscard]] std::string render() const {
        return std::format("( {} ) Accept", checked ? 'x' : ' ');
    }
};

// ----- Dark family -----

/// Dark-theme button.
struct DarkButton {
    [[nodiscard]] std::string render() const { return std::format("<< {} >>", "OK"); }
};

/// Dark-theme checkbox.
struct DarkCheckbox {
    bool checked{};
    [[nodiscard]] std::string render() const {
        return std::format("[{}] Accept", checked ? '*' : '-');
    }
};

} // namespace mcpp::abstract_factory

#endif // MCPP_ABSTRACT_FACTORY_WIDGETS_HPP
