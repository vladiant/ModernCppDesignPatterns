/// \file abstract_factory_test.cpp
/// \brief Catch2 tests for the Abstract Factory pattern.

#include <mcpp/abstract_factory/factories.hpp>
#include <mcpp/abstract_factory/widgets.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace mcpp::abstract_factory;

TEST_CASE("Concepts classify factories and products", "[abstract_factory][concept]") {
    STATIC_REQUIRE(WidgetFactory<LightTheme>);
    STATIC_REQUIRE(WidgetFactory<DarkTheme>);
    STATIC_REQUIRE_FALSE(WidgetFactory<int>);

    STATIC_REQUIRE(Button<LightButton>);
    STATIC_REQUIRE(Button<DarkButton>);
    STATIC_REQUIRE(Checkbox<LightCheckbox>);
    STATIC_REQUIRE(Checkbox<DarkCheckbox>);
}

TEST_CASE("Individual products render correctly", "[abstract_factory][product]") {
    REQUIRE(LightButton{}.render() == "[ OK ]");
    REQUIRE(DarkButton{}.render() == "<< OK >>");
    REQUIRE(LightCheckbox{true}.render() == "( x ) Accept");
    REQUIRE(LightCheckbox{false}.render() == "(   ) Accept");
    REQUIRE(DarkCheckbox{true}.render() == "[*] Accept");
    REQUIRE(DarkCheckbox{false}.render() == "[-] Accept");
}

TEST_CASE("Generic client renders a dialog per family", "[abstract_factory][client]") {
    REQUIRE(render_dialog(LightTheme{}) == "Dialog{ [ OK ] | ( x ) Accept }");
    REQUIRE(render_dialog(DarkTheme{}) == "Dialog{ << OK >> | [*] Accept }");
}
