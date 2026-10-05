#ifndef MCPP_OBSERVER_OBSERVER_HPP
#define MCPP_OBSERVER_OBSERVER_HPP

/// \file observer.hpp
/// \brief Observer pattern with `operator<=>`-driven ordering/dedup (C++20).

#include <compare>
#include <functional>
#include <string>
#include <utility>

namespace mcpp::observer {

/// A broadcast event: a topic plus an opaque payload string.
struct Event {
    std::string topic;
    std::string payload;
};

/// A subscriber. Identity is `(priority, name)`; the callback is intentionally
/// excluded from identity so two handles with the same (priority, name) are
/// considered the same subscriber (enables dedup).
///
/// \note The callback may capture external state by reference; that state must
///       outlive any `notify()` call.
class Observer {
public:
    Observer(int priority, std::string name, std::function<void(const Event&)> on_event)
        : priority_(priority), name_(std::move(name)), on_event_(std::move(on_event)) {}

    /// Order by (priority ascending, then name). Drives both dispatch order and
    /// dedup. The callback is excluded from the comparison.
    [[nodiscard]] std::strong_ordering operator<=>(const Observer& rhs) const {
        if (auto cmp = priority_ <=> rhs.priority_; cmp != 0) {
            return cmp;
        }
        return name_ <=> rhs.name_;
    }

    /// Two observers are equal iff their (priority, name) match.
    [[nodiscard]] bool operator==(const Observer& rhs) const {
        return priority_ == rhs.priority_ && name_ == rhs.name_;
    }

    [[nodiscard]] int priority() const { return priority_; }
    [[nodiscard]] const std::string& name() const { return name_; }

    /// Invoke the callback with \p e.
    void notify(const Event& e) const {
        if (on_event_) {
            on_event_(e);
        }
    }

private:
    int priority_;
    std::string name_;
    std::function<void(const Event&)> on_event_;
};

} // namespace mcpp::observer

#endif // MCPP_OBSERVER_OBSERVER_HPP
