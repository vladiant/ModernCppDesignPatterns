/// \file factory_method_test.cpp
/// \brief Catch2 tests for Factory Method (C2): move_only_function creators,
///        std::expected results, unknown key → std::unexpected.

#include <mcpp/creational26/factory_method.hpp>

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>

using namespace mcpp::creational26;

TEST_CASE("Default factory creates the registered shapes", "[factory]") {
    ShapeFactory factory = make_default_shape_factory();

    auto square = factory.create("square", 3.0);
    REQUIRE(square.has_value());
    REQUIRE(square->kind == "square");
    REQUIRE(square->area == 9.0);

    auto circle = factory.create("circle", 1.0);
    REQUIRE(circle.has_value());
    REQUIRE(circle->kind == "circle");
}

TEST_CASE("Unknown key yields std::unexpected(unknown_kind), not null/throw",
          "[factory][error]") {
    ShapeFactory factory = make_default_shape_factory();

    auto result = factory.create("trapezoid", 2.0);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == FactoryError::unknown_kind);
    REQUIRE_FALSE(factory.knows("trapezoid"));
}

TEST_CASE("Creators may be move-only (own a resource)", "[factory][edge]") {
    ShapeFactory factory;
    // The creator captures a unique_ptr (a non-copyable resource), so it can
    // only be stored because the registry holds std::move_only_function, not a
    // std::function. The resource is read-only inside the const creator.
    factory.register_creator(
        "scaled",
        [factor = std::make_unique<double>(3.0)](double s) -> Shape {
            return {"scaled", s * *factor};
        });

    REQUIRE(factory.knows("scaled"));
    auto result = factory.create("scaled", 10.0);
    REQUIRE(result.has_value());
    REQUIRE(result->area == 30.0);
}

TEST_CASE("register_creator replaces an existing creator", "[factory][edge]") {
    ShapeFactory factory = make_default_shape_factory();
    factory.register_creator(
        "square", [](double) -> Shape { return {"square", -1.0}; });

    auto square = factory.create("square", 5.0);
    REQUIRE(square.has_value());
    REQUIRE(square->area == -1.0);
}
