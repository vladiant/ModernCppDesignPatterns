/// \file bridge_test.cpp
/// \brief Catch2 tests for Bridge (C7): abstraction holds its implementor by
///        value as gof::polymorphic<Renderer>; copies are independent.

#include <mcpp/structural26/bridge.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace mcpp::structural26;

TEST_CASE("Window delegates to its vector implementor", "[bridge]") {
    Window w{"Circle", VectorRenderer{}};
    REQUIRE(w.draw() == "Circle: drawing Circle as vector paths");
}

TEST_CASE("Window delegates to its raster implementor", "[bridge]") {
    Window w{"Square", RasterRenderer{}};
    REQUIRE(w.draw() == "Square: rasterizing Square to pixels");
}

TEST_CASE("Copying a Window deep-copies the implementor", "[bridge]") {
    Window original{"Shape", VectorRenderer{}};
    Window copy = original;
    // Mutate the copy's implementor; the original must be unaffected.
    copy.set_renderer(RasterRenderer{});
    REQUIRE(original.draw() == "Shape: drawing Shape as vector paths");
    REQUIRE(copy.draw() == "Shape: rasterizing Shape to pixels");
}

TEST_CASE("Assigning a Window replaces its implementor independently",
          "[bridge][edge]") {
    Window a{"A", VectorRenderer{}};
    Window b{"B", RasterRenderer{}};
    a = b;
    REQUIRE(a.draw() == "B: rasterizing B to pixels");
    a.set_renderer(VectorRenderer{});
    REQUIRE(b.draw() == "B: rasterizing B to pixels");  // b untouched
}
