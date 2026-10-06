/// \file memento_test.cpp
/// \brief Catch2 tests for Memento (C21): plain-value snapshot save/restore.

#include <mcpp/behavioral26/memento.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace mcpp::behavioral26;

TEST_CASE("restore reverts to a saved snapshot", "[memento]") {
    Editor editor;
    editor.type("Hello");
    auto checkpoint = editor.save();
    editor.type(", world");
    REQUIRE(editor.text() == "Hello, world");

    editor.restore(checkpoint);
    REQUIRE(editor.text() == "Hello");
}

TEST_CASE("snapshot captures the cursor position too", "[memento]") {
    Editor editor;
    editor.type("abc");
    auto snap = editor.save();
    REQUIRE(snap.cursor == 3);
    REQUIRE(snap.text == "abc");

    editor.type("def");
    REQUIRE(editor.cursor() == 6);
    editor.restore(snap);
    REQUIRE(editor.cursor() == 3);
}

TEST_CASE("snapshots are independent value copies", "[memento][edge]") {
    Editor editor;
    editor.type("one");
    auto a = editor.save();
    editor.type("two");
    auto b = editor.save();

    editor.restore(a);
    REQUIRE(editor.text() == "one");
    editor.restore(b);
    REQUIRE(editor.text() == "onetwo");
}
