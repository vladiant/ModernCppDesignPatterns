/// \file chain.hpp
/// \brief Chain of Responsibility (C20) — handlers are move-only callables
///        returning `std::optional`; the first engaged result wins.

#ifndef MCPP_BEHAVIORAL26_CHAIN_HPP
#define MCPP_BEHAVIORAL26_CHAIN_HPP

#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace mcpp::behavioral26 {

/// An approval request: who is asking and for how much.
struct Request {
    std::string who;
    int amount;
};

/// A chain of handlers. `handle` returns the first engaged (`std::optional`
/// non-`nullopt`) result, or a default rejection message if none engage.
class ApprovalChain {
public:
    using Handler =
        std::move_only_function<std::optional<std::string>(const Request&) const>;

    /// Append a handler; returns `*this` for fluent chaining.
    ApprovalChain& then(Handler h) {
        handlers_.push_back(std::move(h));
        return *this;
    }

    std::string handle(const Request& r) const {
        for (const auto& h : handlers_) {
            if (auto result = h(r)) return *result;
        }
        return "rejected: nobody can approve " + std::to_string(r.amount);
    }

private:
    std::vector<Handler> handlers_;
};

}  // namespace mcpp::behavioral26

#endif  // MCPP_BEHAVIORAL26_CHAIN_HPP
