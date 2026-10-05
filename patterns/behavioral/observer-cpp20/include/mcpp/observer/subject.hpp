#ifndef MCPP_OBSERVER_SUBJECT_HPP
#define MCPP_OBSERVER_SUBJECT_HPP

/// \file subject.hpp
/// \brief The Subject that broadcasts events to `<=>`-ordered observers.

#include <mcpp/observer/observer.hpp>

#include <algorithm>
#include <ranges>
#include <vector>

namespace mcpp::observer {

/// Broadcasts `Event`s to its `Observer`s in `operator<=>` order. The observer
/// collection is kept sorted; duplicate (equal under `<=>`) subscriptions are
/// rejected.
class Subject {
public:
    /// Subscribe \p obs. Returns `false` (and does nothing) if an equal observer
    /// is already present (dedup).
    bool subscribe(Observer obs) {
        auto pos = std::ranges::lower_bound(observers_, obs);
        if (pos != observers_.end() && *pos == obs) {
            return false; // duplicate
        }
        observers_.insert(pos, std::move(obs));
        return true;
    }

    /// Unsubscribe the observer equal to \p obs. Returns `false` if not found.
    bool unsubscribe(const Observer& obs) {
        auto pos = std::ranges::lower_bound(observers_, obs);
        if (pos != observers_.end() && *pos == obs) {
            observers_.erase(pos);
            return true;
        }
        return false;
    }

    /// Dispatch \p e to every observer in `<=>` order.
    void publish(const Event& e) const {
        std::ranges::for_each(observers_, [&e](const Observer& o) { o.notify(e); });
    }

    /// Number of current subscribers.
    [[nodiscard]] std::size_t size() const { return observers_.size(); }

private:
    std::vector<Observer> observers_; // kept sorted by operator<=>
};

} // namespace mcpp::observer

#endif // MCPP_OBSERVER_SUBJECT_HPP
