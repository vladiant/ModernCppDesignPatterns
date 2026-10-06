/// \file bridge.hpp
/// \brief Bridge (C7) — the abstraction holds its implementor by value as a
///        `gof::polymorphic<Renderer>` member. The bridge is value-semantic:
///        copying the abstraction deep-copies its implementor (no raw/unique
///        pointer to the impl, no virtual clone()).

#ifndef MCPP_STRUCTURAL26_BRIDGE_HPP
#define MCPP_STRUCTURAL26_BRIDGE_HPP

#include "compat.hpp"

#include <format>
#include <string>
#include <utility>

namespace mcpp::structural26 {

/// Implementor interface (the "bridge" side). Concrete renderers derive from it.
class Renderer {
public:
    virtual ~Renderer() = default;
    /// Render a shape named `shape` into this renderer's notation.
    [[nodiscard]] virtual std::string render(const std::string& shape) const = 0;
};

/// Draws using a vector/SVG-like notation.
class VectorRenderer final : public Renderer {
public:
    [[nodiscard]] std::string render(const std::string& shape) const override {
        return std::format("drawing {} as vector paths", shape);
    }
};

/// Draws using a raster/pixel notation.
class RasterRenderer final : public Renderer {
public:
    [[nodiscard]] std::string render(const std::string& shape) const override {
        return std::format("rasterizing {} to pixels", shape);
    }
};

/// Abstraction side. Owns its implementor **by value** via `gof::polymorphic`,
/// so a `Window` copy is fully independent and deep-copies the renderer.
class Window {
public:
    template <class R>
    Window(std::string title, R renderer)
        : title_(std::move(title)),
          renderer_(gof::polymorphic<Renderer>(std::move(renderer))) {}

    [[nodiscard]] std::string draw() const {
        return title_ + ": " + renderer_->render(title_);
    }

    [[nodiscard]] const std::string& title() const { return title_; }

    /// Swap in a different implementor at runtime (still by value).
    template <class R>
    void set_renderer(R renderer) {
        renderer_ = gof::polymorphic<Renderer>(std::move(renderer));
    }

private:
    std::string title_;
    gof::polymorphic<Renderer> renderer_;
};

}  // namespace mcpp::structural26

#endif  // MCPP_STRUCTURAL26_BRIDGE_HPP
