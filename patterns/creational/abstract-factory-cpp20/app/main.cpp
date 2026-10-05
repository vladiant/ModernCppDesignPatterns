/// \file main.cpp
/// \brief Abstract Factory demo: concept-constrained UI theme families.

#include <mcpp/abstract_factory/factories.hpp>
#include <mcpp/abstract_factory/widgets.hpp>

#include <iostream>

int main() {
    using namespace mcpp::abstract_factory;

    std::cout << "Light theme: " << render_dialog(LightTheme{}) << '\n';
    std::cout << "Dark theme:  " << render_dialog(DarkTheme{}) << '\n';

    // The "abstract" contract is a concept, enforced at compile time:
    static_assert(WidgetFactory<LightTheme>);
    static_assert(WidgetFactory<DarkTheme>);
    static_assert(!WidgetFactory<int>);
    // A wrong-family mix is impossible: `render_dialog(int{})` would fail to
    // compile because `int` does not satisfy WidgetFactory.

    return 0;
}
