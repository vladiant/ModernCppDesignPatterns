#ifndef MCPP_COMMAND_COMMAND_STACK_HPP
#define MCPP_COMMAND_COMMAND_STACK_HPP

/// \file command_stack.hpp
/// \brief The invoker: an undo history of applied commands (C++23).

#include <mcpp/command/commands.hpp>

#include <expected>
#include <functional>
#include <vector>

namespace mcpp::command {

/// Type-erased action over an `Accumulator`.
using Action = std::function<std::expected<void, CommandError>(Accumulator&)>;

/// Owns a history of applied commands and supports undo. Type-erasing commands
/// into `std::function` pairs keeps the invoker non-templated and testable.
class CommandStack {
public:
    /// Run \p cmd against \p acc. On success, push \p undo onto the history.
    /// On failure, nothing is recorded and the error is propagated.
    [[nodiscard]] std::expected<void, CommandError> run(Action cmd, Action undo, Accumulator& acc) {
        auto result = cmd(acc);
        if (!result) {
            return result;
        }
        undo_stack_.push_back(std::move(undo));
        return {};
    }

    /// Undo the most recent command. Returns `nothing_to_undo` if empty.
    [[nodiscard]] std::expected<void, CommandError> undo(Accumulator& acc) {
        if (undo_stack_.empty()) {
            return std::unexpected(CommandError::nothing_to_undo);
        }
        Action action = std::move(undo_stack_.back());
        undo_stack_.pop_back();
        return action(acc);
    }

    /// Number of undoable commands currently recorded.
    [[nodiscard]] std::size_t depth() const { return undo_stack_.size(); }

private:
    std::vector<Action> undo_stack_;
};

} // namespace mcpp::command

#endif // MCPP_COMMAND_COMMAND_STACK_HPP
