/// \file state.hpp
/// \brief State (C16) — states and events are plain structs in variants; the
///        transition table is one `std::visit(gof::overloaded{...})`.

#ifndef MCPP_BEHAVIORAL26_STATE_HPP
#define MCPP_BEHAVIORAL26_STATE_HPP

#include "compat.hpp"

#include <format>
#include <string>
#include <variant>

namespace mcpp::behavioral26 {

// --- states ---
struct Idle {};
struct Playing {
    int track;
};
struct Paused {
    int track;
};
using PlayerState = std::variant<Idle, Playing, Paused>;

// --- events ---
struct Play {};
struct Pause {};
struct Stop {};
using PlayerEvent = std::variant<Play, Pause, Stop>;

/// Resolve the next state from `(state, event)` using a single overload set.
inline PlayerState transition(const PlayerState& s, const PlayerEvent& e) {
    return std::visit(
        gof::overloaded{
            [](const Idle&, const Play&) -> PlayerState { return Playing{1}; },
            [](const Playing& p, const Pause&) -> PlayerState {
                return Paused{p.track};
            },
            [](const Paused& p, const Play&) -> PlayerState {
                return Playing{p.track};
            },
            [](const auto&, const Stop&) -> PlayerState { return Idle{}; },
            [](const auto& st, const auto&) -> PlayerState {
                return st;  // ignore the rest
            },
        },
        s, e);
}

/// Human-readable description of a player state.
inline std::string describe(const PlayerState& s) {
    return std::visit(
        gof::overloaded{
            [](const Idle&) { return std::string{"idle"}; },
            [](const Playing& p) { return std::format("playing track {}", p.track); },
            [](const Paused& p) { return std::format("paused on track {}", p.track); }},
        s);
}

}  // namespace mcpp::behavioral26

#endif  // MCPP_BEHAVIORAL26_STATE_HPP
