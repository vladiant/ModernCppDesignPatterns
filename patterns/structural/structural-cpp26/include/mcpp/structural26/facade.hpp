/// \file facade.hpp
/// \brief Facade (C10) — an `OrderFacade` presents one `place()` call over
///        three subsystems (validate → charge → ship). The steps are chained
///        with `std::expected` monadic operations (`and_then` / `transform`),
///        flattening what would otherwise be nested if-error checks.

#ifndef MCPP_STRUCTURAL26_FACADE_HPP
#define MCPP_STRUCTURAL26_FACADE_HPP

#include "compat.hpp"

#include <expected>
#include <string>
#include <utility>

namespace mcpp::structural26 {

/// A customer order presented to the facade.
struct Order {
    std::string item;
    int quantity;
    double balance;  ///< funds available to charge
};

/// Error categories surfaced by any subsystem step.
enum class OrderError {
    empty_cart,
    out_of_stock,
    insufficient_funds,
    no_carrier,
};

/// Human-readable name for an `OrderError`.
inline std::string to_string(OrderError e) {
    switch (e) {
        case OrderError::empty_cart: return "empty_cart";
        case OrderError::out_of_stock: return "out_of_stock";
        case OrderError::insufficient_funds: return "insufficient_funds";
        case OrderError::no_carrier: return "no_carrier";
    }
    return "unknown";
}

/// Intermediate value carried between the charge and ship steps.
struct Charged {
    Order order;
    double amount_charged;
};

/// A confirmed shipment — the final result of the facade pipeline.
struct Shipment {
    std::string tracking;
    double amount_charged;
};

// --- Subsystems (each a small step returning std::expected) ---------------

/// Subsystem 1: validate the cart is non-empty and in stock.
inline std::expected<Order, OrderError> validate(Order order) {
    if (order.quantity <= 0) return std::unexpected(OrderError::empty_cart);
    if (order.quantity > 10) return std::unexpected(OrderError::out_of_stock);
    return order;
}

/// Subsystem 2: charge the order (unit price fixed at 9.99).
inline std::expected<Charged, OrderError> charge(Order order) {
    const double amount = order.quantity * 9.99;
    if (order.balance < amount)
        return std::unexpected(OrderError::insufficient_funds);
    return Charged{std::move(order), amount};
}

/// Subsystem 3: ship a charged order, producing a tracking number.
inline std::expected<Shipment, OrderError> ship(Charged charged) {
    if (charged.order.item.empty())
        return std::unexpected(OrderError::no_carrier);
    return Shipment{"TRK-" + charged.order.item, charged.amount_charged};
}

/// The facade: one call chains the three subsystems with monadic `and_then`,
/// so no nested `if (error)` ladders appear. Any step's `std::unexpected`
/// short-circuits the rest.
class OrderFacade {
public:
    [[nodiscard]] std::expected<Shipment, OrderError> place(Order order) const {
        return validate(std::move(order))
            .and_then(charge)
            .and_then(ship);
    }

    /// A projection helper showing `transform`: map a success to a receipt
    /// string while preserving the error channel.
    [[nodiscard]] std::expected<std::string, OrderError> place_receipt(
        Order order) const {
        return place(std::move(order)).transform([](const Shipment& s) {
            return "shipped " + s.tracking + " charged " +
                   std::to_string(s.amount_charged);
        });
    }
};

}  // namespace mcpp::structural26

#endif  // MCPP_STRUCTURAL26_FACADE_HPP
