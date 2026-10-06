/// \file abstract_factory_test.cpp
/// \brief Catch2 tests for Abstract Factory (C3): concept-constrained product
///        families, no virtual factory hierarchy.

#include <mcpp/creational26/abstract_factory.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace mcpp::creational26;

// The idiom under test: the "target interface" is a *concept*. Both themes
// satisfy WidgetFactory and their products satisfy the product concepts.
static_assert(Button<LightButton>);
static_assert(Checkbox<LightCheckbox>);
static_assert(Button<DarkButton>);
static_assert(Checkbox<DarkCheckbox>);
static_assert(WidgetFactory<LightTheme>);
static_assert(WidgetFactory<DarkTheme>);

// A type lacking the factory methods does NOT satisfy the concept.
struct NotAFactory {};
static_assert(!WidgetFactory<NotAFactory>);

TEST_CASE("render_dialog uses the light family", "[abstract_factory]") {
    const std::string dialog = render_dialog(LightTheme{});
    REQUIRE(dialog.find("Light Button") != std::string::npos);
    REQUIRE(dialog.find("Light Checkbox") != std::string::npos);
}

TEST_CASE("render_dialog uses the dark family", "[abstract_factory]") {
    const std::string dialog = render_dialog(DarkTheme{});
    REQUIRE(dialog.find("Dark Button") != std::string::npos);
    REQUIRE(dialog.find("Dark Checkbox") != std::string::npos);
}

TEST_CASE("families produce consistent products in isolation",
          "[abstract_factory][edge]") {
    LightTheme light;
    DarkTheme dark;
    REQUIRE(light.make_button().render() != dark.make_button().render());
    REQUIRE(light.make_checkbox().render() != dark.make_checkbox().render());
}
