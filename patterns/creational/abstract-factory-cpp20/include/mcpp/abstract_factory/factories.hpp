#ifndef MCPP_ABSTRACT_FACTORY_FACTORIES_HPP
#define MCPP_ABSTRACT_FACTORY_FACTORIES_HPP

/// \file factories.hpp
/// \brief Abstract Factory families + generic concept-constrained client.

#include <mcpp/abstract_factory/widgets.hpp>

#include <concepts>
#include <format>
#include <string>
#include <utility>

namespace mcpp::abstract_factory {

/// Abstract-factory concept: a family must make a Button and a Checkbox.
template <class F>
concept WidgetFactory = requires(const F& f) {
    { f.make_button() };
    { f.make_checkbox() };
} && Button<decltype(std::declval<const F&>().make_button())> &&
    Checkbox<decltype(std::declval<const F&>().make_checkbox())>;

/// Concrete factory for the Light family.
struct LightTheme {
    [[nodiscard]] LightButton make_button() const { return LightButton{}; }
    [[nodiscard]] LightCheckbox make_checkbox() const { return LightCheckbox{true}; }
};

/// Concrete factory for the Dark family.
struct DarkTheme {
    [[nodiscard]] DarkButton make_button() const { return DarkButton{}; }
    [[nodiscard]] DarkCheckbox make_checkbox() const { return DarkCheckbox{true}; }
};

/// Generic client constrained on the abstract-factory concept. Lays out a
/// dialog using whichever family it is given. A wrong-family mix (or a
/// non-factory type) fails to compile, e.g. `render_dialog(int{})`.
template <WidgetFactory F>
[[nodiscard]] std::string render_dialog(const F& factory) {
    const auto button = factory.make_button();
    const auto checkbox = factory.make_checkbox();
    return std::format("Dialog{{ {} | {} }}", button.render(), checkbox.render());
}

} // namespace mcpp::abstract_factory

#endif // MCPP_ABSTRACT_FACTORY_FACTORIES_HPP
