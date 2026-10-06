/// \file reflect_test.cpp
/// \brief Catch2 v3 tests for the Reflection showcase (C22).
///
/// The reflection-dependent assertions are guarded by `__cpp_reflection`. On
/// every compiler available in this repo that macro is undefined, so this file
/// compiles to a single trivially-passing placeholder TEST_CASE — keeping the
/// file a valid, green translation unit (NFR-C26-4). On an experimental P2996
/// compiler the real assertions exercise `to_string`, `field_count`, and
/// `for_each_field` over arbitrary aggregates.

#include <mcpp/reflection26/reflect.hpp>

#include <catch2/catch_test_macros.hpp>

#if defined(__cpp_reflection)

#include <string>
#include <string_view>
#include <vector>

namespace {

struct Point {
    int x;
    int y;
};

struct Person {
    std::string name;
    int age;
    bool active;
};

}  // namespace

using namespace mcpp::reflection26;

TEST_CASE("to_string reflects an aggregate's fields", "[reflection]") {
    const Point p{3, 4};
    REQUIRE(to_string(p) == "Point{ x=3, y=4 }");
}

TEST_CASE("field_count counts non-static data members", "[reflection]") {
    STATIC_REQUIRE(field_count<Point>() == 2);
    STATIC_REQUIRE(field_count<Person>() == 3);
}

TEST_CASE("for_each_field visits every member in order", "[reflection]") {
    const Person person{"Ada", 36, true};
    std::vector<std::string> names;
    for_each_field(person, [&](std::string_view name, const auto&) {
        names.emplace_back(name);
    });
    REQUIRE(names == std::vector<std::string>{"name", "age", "active"});
}

#else  // !defined(__cpp_reflection)

TEST_CASE("reflection showcase is build-gated (no __cpp_reflection)",
          "[reflection][gated]") {
    // Placeholder so this TU is a valid, trivially-passing test when built
    // without an experimental P2996 reflection compiler. CI stays green.
    SUCCEED("built without __cpp_reflection; reflection assertions skipped");
}

#endif  // defined(__cpp_reflection)
