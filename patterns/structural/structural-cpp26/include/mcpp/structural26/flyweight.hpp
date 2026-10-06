/// \file flyweight.hpp
/// \brief Flyweight (C11) — a `GlyphCache` interns immutable glyphs as
///        `std::shared_ptr<const Glyph>`. Repeated requests for the same
///        character return the *same* shared instance (shared intrinsic state).

#ifndef MCPP_STRUCTURAL26_FLYWEIGHT_HPP
#define MCPP_STRUCTURAL26_FLYWEIGHT_HPP

#include "compat.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>

namespace mcpp::structural26 {

/// Immutable intrinsic state shared by every occurrence of a character.
class Glyph {
public:
    Glyph(char code, int advance_width)
        : code_(code), advance_width_(advance_width) {}

    [[nodiscard]] char code() const { return code_; }
    [[nodiscard]] int advance_width() const { return advance_width_; }

private:
    char code_;
    int advance_width_;
};

/// Factory + cache. Hands out `shared_ptr<const Glyph>` so clients cannot
/// mutate shared state, and returns the identical instance for a repeated key.
class GlyphCache {
public:
    /// Return the interned glyph for `code`, creating it on first request.
    [[nodiscard]] std::shared_ptr<const Glyph> get(char code) {
        auto it = cache_.find(code);
        if (it != cache_.end()) return it->second;
        auto glyph = std::make_shared<const Glyph>(code, default_advance(code));
        cache_.emplace(code, glyph);
        return glyph;
    }

    /// Number of distinct glyphs currently interned.
    [[nodiscard]] std::size_t size() const { return cache_.size(); }

private:
    static int default_advance(char code) {
        // Trivial metric: space is narrow, everything else is uniform width.
        return code == ' ' ? 4 : 8;
    }

    std::unordered_map<char, std::shared_ptr<const Glyph>> cache_;
};

/// Lay out a string by interning each character through the cache. Returns the
/// total advance width; the caller can inspect `cache.size()` to see how few
/// distinct glyphs backed a long string.
inline int layout_width(GlyphCache& cache, const std::string& text) {
    int width = 0;
    for (char c : text) width += cache.get(c)->advance_width();
    return width;
}

}  // namespace mcpp::structural26

#endif  // MCPP_STRUCTURAL26_FLYWEIGHT_HPP
