/// \file prototype_test.cpp
/// \brief Catch2 tests for Prototype (C5): gof::polymorphic deep clone (copy ==
///        deep clone of the dynamic type), with no virtual clone().

#include <mcpp/creational26/prototype.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace mcpp::creational26;

TEST_CASE("Cloning deep-copies the dynamic type", "[prototype]") {
    PrototypeRegistry registry;
    registry.register_prototype("circle",
                                gof::polymorphic<Figure>(Circle{"blue", 2.0}));

    auto clone = registry.clone("circle");
    REQUIRE(clone.has_value());
    // Note: C++26 std::to_string uses the shortest round-trip form (P2587),
    // so 2.0 renders as "2"; assert on the stable, meaningful substrings.
    const std::string desc = (*clone)->describe();
    REQUIRE(desc.find("blue") != std::string::npos);
    REQUIRE(desc.find("circle") != std::string::npos);
    REQUIRE(desc.find("r=2") != std::string::npos);
}

TEST_CASE("A clone can be mutated independently of further clones",
          "[prototype]") {
    PrototypeRegistry registry;
    registry.register_prototype("circle",
                                gof::polymorphic<Figure>(Circle{"blue", 1.0}));

    auto a = registry.clone("circle");
    (*a)->set_color("red");

    auto b = registry.clone("circle");  // fresh clone from the prototype
    REQUIRE((*a)->color == "red");
    REQUIRE((*b)->color == "blue");  // prototype was unaffected by a's mutation
}

TEST_CASE("Different dynamic types are preserved across clones",
          "[prototype][edge]") {
    PrototypeRegistry registry;
    registry.register_prototype(
        "rect", gof::polymorphic<Figure>(Rectangle{"white", 4.0, 3.0}));

    auto clone = registry.clone("rect");
    REQUIRE(clone.has_value());
    const std::string desc = (*clone)->describe();
    REQUIRE(desc.find("white") != std::string::npos);
    REQUIRE(desc.find("rectangle") != std::string::npos);
    REQUIRE(desc.find("4x3") != std::string::npos);
}

TEST_CASE("Unknown prototype yields nullopt", "[prototype][error]") {
    PrototypeRegistry registry;
    registry.register_prototype("circle",
                                gof::polymorphic<Figure>(Circle{"blue", 1.0}));

    REQUIRE_FALSE(registry.knows("triangle"));
    REQUIRE_FALSE(registry.clone("triangle").has_value());
}

TEST_CASE("Copying the polymorphic wrapper directly deep-clones", "[prototype]") {
    gof::polymorphic<Figure> original(Circle{"green", 5.0});
    gof::polymorphic<Figure> copy = original;  // deep clone, no virtual clone()

    copy->set_color("yellow");
    REQUIRE(original->color == "green");
    REQUIRE(copy->color == "yellow");
    REQUIRE(original->describe().find("green circle") != std::string::npos);
}
