/// \file abstract_factory.hpp
/// \brief Abstract Factory (C3) — product families selected by *concepts* over
///        theme types, with **no** virtual factory hierarchy.

#ifndef MCPP_CREATIONAL26_ABSTRACT_FACTORY_HPP
#define MCPP_CREATIONAL26_ABSTRACT_FACTORY_HPP

#include <concepts>
#include <string>

namespace mcpp::creational26 {

// --- Product concepts -------------------------------------------------------
// A product is anything that can render itself to a string; there is no shared
// abstract base class — membership in the family is structural (concept-based).

/// A button renders to a string.
template <class B>
concept Button = requires(const B b) {
    { b.render() } -> std::convertible_to<std::string>;
};

/// A checkbox renders to a string.
template <class C>
concept Checkbox = requires(const C c) {
    { c.render() } -> std::convertible_to<std::string>;
};

/// A widget factory makes a `Button` and a `Checkbox`. The *target interface*
/// is this concept, not a virtual base — any theme type that structurally
/// provides the two factory methods (returning conforming products) satisfies
/// it.
template <class F>
concept WidgetFactory = requires(const F f) {
    { f.make_button() } -> Button;
    { f.make_checkbox() } -> Checkbox;
};

// --- Light theme family -----------------------------------------------------

struct LightButton {
    [[nodiscard]] std::string render() const { return "[ Light Button ]"; }
};
struct LightCheckbox {
    [[nodiscard]] std::string render() const { return "( ) Light Checkbox"; }
};

/// Concrete factory for the light theme (no inheritance).
struct LightTheme {
    [[nodiscard]] LightButton make_button() const { return {}; }
    [[nodiscard]] LightCheckbox make_checkbox() const { return {}; }
};

// --- Dark theme family ------------------------------------------------------

struct DarkButton {
    [[nodiscard]] std::string render() const { return "[# Dark Button #]"; }
};
struct DarkCheckbox {
    [[nodiscard]] std::string render() const { return "(x) Dark Checkbox"; }
};

/// Concrete factory for the dark theme (no inheritance).
struct DarkTheme {
    [[nodiscard]] DarkButton make_button() const { return {}; }
    [[nodiscard]] DarkCheckbox make_checkbox() const { return {}; }
};

/// Generic client: render a dialog from *whichever* family it is handed. The
/// `WidgetFactory` constraint is checked at compile time; passing a type that
/// does not satisfy it is a clear compile error, not a runtime failure.
template <WidgetFactory F>
[[nodiscard]] std::string render_dialog(const F& factory) {
    const auto button = factory.make_button();
    const auto checkbox = factory.make_checkbox();
    return "Dialog { " + button.render() + " | " + checkbox.render() + " }";
}

}  // namespace mcpp::creational26

#endif  // MCPP_CREATIONAL26_ABSTRACT_FACTORY_HPP
