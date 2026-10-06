/// \file prototype.hpp
/// \brief Prototype (C5) — a registry of `gof::polymorphic<Figure>` prototypes
///        where **copying the wrapper deep-clones the dynamic type**. There is
///        no virtual `clone()`; a clone is a plain value copy.

#ifndef MCPP_CREATIONAL26_PROTOTYPE_HPP
#define MCPP_CREATIONAL26_PROTOTYPE_HPP

#include "compat.hpp"

#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

namespace mcpp::creational26 {

/// Polymorphic base for things that can be registered as prototypes.
///
/// Note the **absence** of a virtual `clone()`: deep copying is the job of the
/// value-semantic `gof::polymorphic<Figure>` wrapper, not of this hierarchy.
struct Figure {
    std::string color{"black"};

    Figure() = default;
    explicit Figure(std::string c) : color(std::move(c)) {}
    virtual ~Figure() = default;

    /// Human-readable description of the concrete figure.
    [[nodiscard]] virtual std::string describe() const = 0;

    /// Recolor this figure (used to mutate a clone independently).
    void set_color(std::string c) { color = std::move(c); }

protected:
    Figure(const Figure&) = default;
    Figure& operator=(const Figure&) = default;
    Figure(Figure&&) = default;
    Figure& operator=(Figure&&) = default;
};

/// A circle prototype.
struct Circle : Figure {
    double radius{1.0};

    Circle() = default;
    Circle(std::string c, double r) : Figure(std::move(c)), radius(r) {}

    [[nodiscard]] std::string describe() const override {
        return color + " circle r=" + std::to_string(radius);
    }
};

/// A rectangle prototype.
struct Rectangle : Figure {
    double width{1.0};
    double height{1.0};

    Rectangle() = default;
    Rectangle(std::string c, double w, double h)
        : Figure(std::move(c)), width(w), height(h) {}

    [[nodiscard]] std::string describe() const override {
        return color + " rectangle " + std::to_string(width) + "x" +
               std::to_string(height);
    }
};

/// A registry of named prototypes, each stored as a value-semantic
/// `gof::polymorphic<Figure>`. Cloning a registered prototype is simply
/// **copying** the wrapper, which deep-clones the stored dynamic type.
class PrototypeRegistry {
public:
    using Proto = gof::polymorphic<Figure>;

    /// Register (or replace) a prototype under \p name.
    void register_prototype(std::string name, Proto prototype) {
        prototypes_.insert_or_assign(std::move(name), std::move(prototype));
    }

    /// True if a prototype is registered for \p name.
    [[nodiscard]] bool knows(const std::string& name) const {
        return prototypes_.contains(name);
    }

    /// Deep-clone the prototype registered under \p name.
    /// \returns an independent copy, or `std::nullopt` if \p name is unknown.
    ///          The copy is produced by `gof::polymorphic`'s copy constructor —
    ///          no virtual `clone()` is involved.
    [[nodiscard]] std::optional<Proto> clone(const std::string& name) const {
        const auto it = prototypes_.find(name);
        if (it == prototypes_.end()) {
            return std::nullopt;
        }
        return it->second;  // copy == deep clone of the dynamic type
    }

private:
    std::unordered_map<std::string, Proto> prototypes_;
};

}  // namespace mcpp::creational26

#endif  // MCPP_CREATIONAL26_PROTOTYPE_HPP
