/// \file command.hpp
/// \brief Command (C15) — do/undo pairs as move-only closures; `History`
///        drives execute / undo / redo.

#ifndef MCPP_BEHAVIORAL26_COMMAND_HPP
#define MCPP_BEHAVIORAL26_COMMAND_HPP

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace mcpp::behavioral26 {

/// A single reversible action: a label plus `redo`/`undo` move-only closures.
struct Command {
    std::string label;
    std::move_only_function<void()> redo;
    std::move_only_function<void()> undo;
};

/// Undo/redo stack. `execute` runs a command and clears the redo stack; `undo`
/// and `redo` move commands between the done/undone stacks.
class History {
public:
    void execute(Command c) {
        c.redo();
        done_.push_back(std::move(c));
        undone_.clear();
    }

    /// Reverse the most recent command. Returns false if nothing to undo.
    bool undo() {
        if (done_.empty()) return false;
        done_.back().undo();
        undone_.push_back(std::move(done_.back()));
        done_.pop_back();
        return true;
    }

    /// Re-apply the most recently undone command. Returns false if none.
    bool redo() {
        if (undone_.empty()) return false;
        undone_.back().redo();
        done_.push_back(std::move(undone_.back()));
        undone_.pop_back();
        return true;
    }

private:
    std::vector<Command> done_, undone_;
};

}  // namespace mcpp::behavioral26

#endif  // MCPP_BEHAVIORAL26_COMMAND_HPP
