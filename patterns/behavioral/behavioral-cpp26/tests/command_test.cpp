/// \file command_test.cpp
/// \brief Catch2 tests for Command (C15): execute / undo / redo over a document.

#include <mcpp/behavioral26/command.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace mcpp::behavioral26;

namespace {
Command append(std::string& doc, std::string s) {
    return Command{"append " + s, [&doc, s] { doc += s; },
                   [&doc, n = s.size()] { doc.resize(doc.size() - n); }};
}
}  // namespace

TEST_CASE("execute runs commands and accumulates state", "[command]") {
    std::string doc;
    History h;
    h.execute(append(doc, "Hello"));
    h.execute(append(doc, ", world"));
    REQUIRE(doc == "Hello, world");
}

TEST_CASE("undo reverses, redo re-applies", "[command]") {
    std::string doc;
    History h;
    h.execute(append(doc, "Hello"));
    h.execute(append(doc, ", world"));

    REQUIRE(h.undo());
    REQUIRE(doc == "Hello");
    REQUIRE(h.redo());
    REQUIRE(doc == "Hello, world");
}

TEST_CASE("undo/redo on empty stacks return false", "[command][edge]") {
    std::string doc;
    History h;
    REQUIRE_FALSE(h.undo());
    REQUIRE_FALSE(h.redo());
}

TEST_CASE("a new execute clears the redo stack", "[command][edge]") {
    std::string doc;
    History h;
    h.execute(append(doc, "A"));
    h.execute(append(doc, "B"));
    REQUIRE(h.undo());        // doc == "A", "B" on redo stack
    REQUIRE(doc == "A");
    h.execute(append(doc, "C"));  // clears redo
    REQUIRE(doc == "AC");
    REQUIRE_FALSE(h.redo());  // "B" is gone
}
