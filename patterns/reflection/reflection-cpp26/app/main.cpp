/// \file main.cpp
/// \brief Narrative demo for the Reflection showcase (C22).
///
/// Only meaningful on an experimental **P2996 reflection compiler**: the demo
/// body (guarded by `__cpp_reflection`) reflects over arbitrary aggregates and
/// prints them with a single generic `to_string`. Built **without** reflection
/// (the default on every compiler in this repo), it prints a clear note and
/// exits 0, so the demo is harmless if it is ever accidentally compiled.

#include <mcpp/reflection26/reflect.hpp>

#include <iostream>

#if defined(__cpp_reflection)
#include <string>
#endif

namespace {

#if defined(__cpp_reflection)
/// An arbitrary aggregate — note there is NO hand-written `to_string` for it.
struct Point {
    int x;
    int y;
};

/// A second, differently-shaped aggregate — reused by the SAME generic code.
struct Person {
    std::string name;
    int age;
    bool active;
};
#endif

}  // namespace

int main() {
#if defined(__cpp_reflection)
    using mcpp::reflection26::field_count;
    using mcpp::reflection26::for_each_field;
    using mcpp::reflection26::to_string;

    std::cout << "== Reflection showcase (C22, P2996) ==\n";

    const Point p{3, 4};
    std::cout << "to_string(Point): " << to_string(p) << '\n';
    std::cout << "Point field_count: " << field_count<Point>() << '\n';

    const Person person{"Ada", 36, true};
    std::cout << "to_string(Person): " << to_string(person) << '\n';

    std::cout << "Person fields via for_each_field:\n";
    for_each_field(person, [](std::string_view name, const auto& field) {
        std::cout << "  " << name << " = " << field << '\n';
    });

    return 0;
#else
    std::cout
        << "reflection26_demo: built WITHOUT P2996 reflection "
           "(__cpp_reflection is not defined by this compiler).\n"
           "The showcase source is present but requires an experimental "
           "reflection compiler. Exiting 0.\n";
    return 0;
#endif
}
