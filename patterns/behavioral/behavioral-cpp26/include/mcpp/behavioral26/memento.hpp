/// \file memento.hpp
/// \brief Memento (C21) — with value semantics the snapshot is just a copy of
///        the state; no friend-access snapshot class is needed.

#ifndef MCPP_BEHAVIORAL26_MEMENTO_HPP
#define MCPP_BEHAVIORAL26_MEMENTO_HPP

#include <cstddef>
#include <string>
#include <string_view>

namespace mcpp::behavioral26 {

/// A tiny text editor whose entire state is captured by a plain-value snapshot.
class Editor {
public:
    /// A value snapshot of the editor state.
    struct Snapshot {
        std::string text;
        std::size_t cursor;
    };

    /// Insert text at the cursor and advance it.
    void type(std::string_view s) {
        text_.insert(cursor_, s);
        cursor_ += s.size();
    }

    /// Capture the current state.
    [[nodiscard]] Snapshot save() const { return {text_, cursor_}; }

    /// Restore a previously saved state.
    void restore(const Snapshot& s) {
        text_ = s.text;
        cursor_ = s.cursor;
    }

    const std::string& text() const { return text_; }
    std::size_t cursor() const { return cursor_; }

private:
    std::string text_;
    std::size_t cursor_ = 0;
};

}  // namespace mcpp::behavioral26

#endif  // MCPP_BEHAVIORAL26_MEMENTO_HPP
