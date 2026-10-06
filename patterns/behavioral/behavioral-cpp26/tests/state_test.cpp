/// \file state_test.cpp
/// \brief Catch2 tests for State (C16): variant states x events transition
///        table resolved by a single std::visit.

#include <mcpp/behavioral26/state.hpp>

#include <catch2/catch_test_macros.hpp>

#include <variant>

using namespace mcpp::behavioral26;

TEST_CASE("Idle --Play--> Playing track 1", "[state]") {
    PlayerState s = Idle{};
    s = transition(s, Play{});
    REQUIRE(std::holds_alternative<Playing>(s));
    REQUIRE(std::get<Playing>(s).track == 1);
    REQUIRE(describe(s) == "playing track 1");
}

TEST_CASE("Playing --Pause--> Paused keeps the track", "[state]") {
    PlayerState s = Playing{7};
    s = transition(s, Pause{});
    REQUIRE(std::holds_alternative<Paused>(s));
    REQUIRE(std::get<Paused>(s).track == 7);
    REQUIRE(describe(s) == "paused on track 7");
}

TEST_CASE("Paused --Play--> resumes same track", "[state]") {
    PlayerState s = Paused{3};
    s = transition(s, Play{});
    REQUIRE(std::get<Playing>(s).track == 3);
}

TEST_CASE("Stop from any state returns to Idle", "[state]") {
    REQUIRE(describe(transition(Playing{5}, Stop{})) == "idle");
    REQUIRE(describe(transition(Paused{5}, Stop{})) == "idle");
    REQUIRE(describe(transition(Idle{}, Stop{})) == "idle");
}

TEST_CASE("Unhandled events leave the state unchanged", "[state][edge]") {
    // Pause while Idle is not a valid transition -> stays Idle.
    PlayerState s = Idle{};
    s = transition(s, Pause{});
    REQUIRE(describe(s) == "idle");
}

TEST_CASE("Full narrative sequence matches the demo", "[state]") {
    PlayerState s = Idle{};
    s = transition(s, Play{});
    REQUIRE(describe(s) == "playing track 1");
    s = transition(s, Pause{});
    REQUIRE(describe(s) == "paused on track 1");
    s = transition(s, Play{});
    REQUIRE(describe(s) == "playing track 1");
    s = transition(s, Stop{});
    REQUIRE(describe(s) == "idle");
    s = transition(s, Pause{});
    REQUIRE(describe(s) == "idle");
}
