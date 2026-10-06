/// \file factory_method.hpp
/// \brief Factory Method (C2) — a registry of `std::move_only_function`
///        creators keyed by name; `create()` returns `std::expected<Shape,
///        FactoryError>` (unknown key → `std::unexpected`, never null/throw).

#ifndef MCPP_CREATIONAL26_FACTORY_METHOD_HPP
#define MCPP_CREATIONAL26_FACTORY_METHOD_HPP

#include <expected>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>

namespace mcpp::creational26 {

/// The product assembled by the factory.
struct Shape {
    std::string kind;  ///< e.g. "circle", "square", "hexagon".
    double area{};     ///< computed from the requested size.

    /// Human-readable one-liner for demo output.
    [[nodiscard]] std::string describe() const {
        return kind + " (area=" + std::to_string(area) + ")";
    }

    bool operator==(const Shape&) const = default;
};

/// Why a `create()` call failed — returned instead of throwing or returning
/// null.
enum class FactoryError {
    unknown_kind,  ///< no creator registered for the requested name.
};

/// A registry mapping a shape name to a **move-only** creator closure, so a
/// creator may own non-copyable resources (e.g. a captured `unique_ptr`).
class ShapeFactory {
public:
    /// Signature of a creator: given a size, produce a `Shape`.
    using Creator = std::move_only_function<Shape(double) const>;

    /// Register (or replace) the creator for \p name.
    void register_creator(std::string name, Creator creator) {
        creators_.insert_or_assign(std::move(name), std::move(creator));
    }

    /// True if a creator is registered for \p name.
    [[nodiscard]] bool knows(const std::string& name) const {
        return creators_.contains(name);
    }

    /// Create a shape by name.
    /// \returns the product, or `std::unexpected(FactoryError::unknown_kind)`
    ///          when \p name is not registered. Never null, never throws.
    [[nodiscard]] std::expected<Shape, FactoryError> create(
        const std::string& name, double size) const {
        const auto it = creators_.find(name);
        if (it == creators_.end()) {
            return std::unexpected(FactoryError::unknown_kind);
        }
        return it->second(size);
    }

private:
    std::unordered_map<std::string, Creator> creators_;
};

/// Build a factory pre-populated with the standard built-in shapes. Convenient
/// for the demo and tests.
[[nodiscard]] inline ShapeFactory make_default_shape_factory() {
    ShapeFactory factory;
    factory.register_creator(
        "circle", [](double r) -> Shape { return {"circle", 3.14159265 * r * r}; });
    factory.register_creator(
        "square", [](double s) -> Shape { return {"square", s * s}; });
    factory.register_creator("hexagon", [](double s) -> Shape {
        return {"hexagon", 2.59807621 * s * s};  // (3*sqrt(3)/2) * s^2
    });
    return factory;
}

}  // namespace mcpp::creational26

#endif  // MCPP_CREATIONAL26_FACTORY_METHOD_HPP
