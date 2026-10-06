/// \file observer_test.cpp
/// \brief Catch2 tests for Observer (C14): Signal connect/emit/disconnect.

#include <mcpp/behavioral26/observer.hpp>

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

using namespace mcpp::behavioral26;

TEST_CASE("connect + emit invokes every slot in order", "[observer]") {
    Signal<int> sig;
    std::vector<int> seen;
    sig.connect([&](int x) { seen.push_back(x); });
    sig.connect([&](int x) { seen.push_back(x * 10); });

    sig.emit(2);
    REQUIRE(seen == std::vector<int>{2, 20});
}

TEST_CASE("disconnect removes a slot by id", "[observer]") {
    Signal<std::string_view, int> sig;
    int logger_calls = 0;
    auto logger = sig.connect([&](std::string_view, int) { ++logger_calls; });
    int other_calls = 0;
    sig.connect([&](std::string_view, int) { ++other_calls; });

    sig.emit("ACME", 90);
    REQUIRE(logger_calls == 1);
    REQUIRE(other_calls == 1);

    sig.disconnect(logger);
    sig.emit("ACME", 130);
    REQUIRE(logger_calls == 1);  // unchanged
    REQUIRE(other_calls == 2);
}

TEST_CASE("disconnecting an unknown id is a no-op", "[observer][edge]") {
    Signal<int> sig;
    int calls = 0;
    sig.connect([&](int) { ++calls; });
    sig.disconnect(999);
    sig.emit(1);
    REQUIRE(calls == 1);
}

TEST_CASE("slots may own move-only state", "[observer][edge]") {
    Signal<int> sig;
    sig.connect([counter = std::make_unique<int>(0)](int x) mutable {
        *counter += x;
    });
    sig.emit(3);
    sig.emit(4);
    SUCCEED("move-only slot invoked without copy");
}
