/// \file main.cpp
/// \brief Decorator demo: type-preserving text pipeline (deducing this +
///        static operator()).

#include <mcpp/decorator/pipeline.hpp>

#include <format>
#include <iostream>
#include <string>

int main() {
    using namespace mcpp::decorator;

    // Build a concrete, fully-typed composite at compile time:
    //   Identity -> Trim -> Uppercase -> Prefix
    auto pipeline = decorate(Trim{}).with(Uppercase{}).with(Prefix{"[LOG] "});

    const std::string input = "   hello, decorator   ";
    std::cout << std::format("input   = '{}'\n", input);
    std::cout << std::format("output  = '{}'\n", pipeline(input));

    // Each stateless layer can be used stand-alone via its static operator().
    std::cout << std::format("Uppercase('abc') = '{}'\n", Uppercase{}("abc"));
    std::cout << std::format("Trim('  x  ')    = '{}'\n", Trim{}("  x  "));

    // The composite is a plain value type (no type erasure); it can be copied,
    // stored, and reused:
    auto reused = pipeline;
    std::cout << std::format("reused('  a b ') = '{}'\n", reused("  a b "));

    return 0;
}
