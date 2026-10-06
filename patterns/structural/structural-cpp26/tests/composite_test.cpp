/// \file composite_test.cpp
/// \brief Catch2 tests for Composite (C8): a std::variant tree whose totals are
///        computed by a self-recursive deducing-this lambda.

#include <mcpp/structural26/composite.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace mcpp::structural26;

TEST_CASE("A single leaf reports its own size", "[composite][edge]") {
    Node n = file("a.txt", 42);
    REQUIRE(total_size(n) == 42);
    REQUIRE(count_files(n) == 1);
}

TEST_CASE("An empty directory has zero size and no files", "[composite][edge]") {
    Node n = directory("empty", {});
    REQUIRE(total_size(n) == 0);
    REQUIRE(count_files(n) == 0);
}

TEST_CASE("A nested tree sums all leaf sizes recursively", "[composite]") {
    Node tree = directory(
        "root",
        {file("readme.md", 120),
         directory("src", {file("main.cpp", 900), file("util.hpp", 300)}),
         file("LICENSE", 1100)});
    REQUIRE(total_size(tree) == 120 + 900 + 300 + 1100);
    REQUIRE(count_files(tree) == 4);
}

TEST_CASE("render produces an indented listing", "[composite]") {
    Node tree =
        directory("root", {file("a", 1), directory("sub", {file("b", 2)})});
    const std::string expected =
        "root/\n"
        "  a (1)\n"
        "  sub/\n"
        "    b (2)\n";
    REQUIRE(render(tree) == expected);
}
