/// \file flyweight_test.cpp
/// \brief Catch2 tests for Flyweight (C11): a glyph cache interns immutable
///        glyphs as shared_ptr<const Glyph>; repeats share one instance.

#include <mcpp/structural26/flyweight.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace mcpp::structural26;

TEST_CASE("Repeated requests return the identical shared instance",
          "[flyweight]") {
    GlyphCache cache;
    auto a = cache.get('x');
    auto b = cache.get('x');
    REQUIRE(a.get() == b.get());  // same object, shared
    REQUIRE(cache.size() == 1);
}

TEST_CASE("Distinct characters produce distinct glyphs", "[flyweight]") {
    GlyphCache cache;
    auto x = cache.get('x');
    auto y = cache.get('y');
    REQUIRE(x.get() != y.get());
    REQUIRE(cache.size() == 2);
}

TEST_CASE("Interned glyphs carry immutable intrinsic state", "[flyweight]") {
    GlyphCache cache;
    auto space = cache.get(' ');
    auto letter = cache.get('a');
    REQUIRE(space->advance_width() == 4);
    REQUIRE(letter->advance_width() == 8);
    REQUIRE(space->code() == ' ');
}

TEST_CASE("A long string reuses few distinct glyphs", "[flyweight]") {
    GlyphCache cache;
    const int width = layout_width(cache, "mississippi");
    // 'm','i','s','p' are the only 4 distinct characters.
    REQUIRE(cache.size() == 4);
    REQUIRE(width == 11 * 8);
}

TEST_CASE("An empty string interns nothing", "[flyweight][edge]") {
    GlyphCache cache;
    REQUIRE(layout_width(cache, "") == 0);
    REQUIRE(cache.size() == 0);
}
