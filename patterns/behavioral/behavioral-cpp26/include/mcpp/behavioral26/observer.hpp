/// \file observer.hpp
/// \brief Observer (C14) — a tiny `Signal<Args...>` whose slots are move-only
///        callables (they may own resources).

#ifndef MCPP_BEHAVIORAL26_OBSERVER_HPP
#define MCPP_BEHAVIORAL26_OBSERVER_HPP

#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

namespace mcpp::behavioral26 {

/// A minimal signal/slot type. Slots are `std::move_only_function`, so a slot
/// may own non-copyable state (e.g. a `std::unique_ptr`). `connect` returns an
/// id usable with `disconnect`.
template <class... Args>
class Signal {
public:
    using Slot = std::move_only_function<void(Args...)>;

    /// Register a slot; returns its connection id.
    std::size_t connect(Slot s) {
        slots_.emplace_back(next_id_, std::move(s));
        return next_id_++;
    }

    /// Remove the slot previously registered under `id` (no-op if absent).
    void disconnect(std::size_t id) {
        std::erase_if(slots_, [id](const auto& p) { return p.first == id; });
    }

    /// Invoke every connected slot with `args...` in connection order.
    void emit(Args... args) {
        for (auto& [id, slot] : slots_) slot(args...);
    }

private:
    std::vector<std::pair<std::size_t, Slot>> slots_;
    std::size_t next_id_ = 0;
};

}  // namespace mcpp::behavioral26

#endif  // MCPP_BEHAVIORAL26_OBSERVER_HPP
