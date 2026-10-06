/// \file composite.hpp
/// \brief Composite (C8) — a filesystem tree modelled as a `std::variant`
///        (`File` / `Directory`), with totals/rendering computed by a
///        self-recursive lambda using *deducing this* (`this auto&& self`).
///        No virtual `Component` hierarchy.

#ifndef MCPP_STRUCTURAL26_COMPOSITE_HPP
#define MCPP_STRUCTURAL26_COMPOSITE_HPP

#include "compat.hpp"

#include <cstddef>
#include <string>
#include <variant>
#include <vector>

namespace mcpp::structural26 {

struct File;
struct Directory;

/// A node in the tree is either a `File` (leaf) or a `Directory` (group).
using Node = std::variant<File, Directory>;

/// Leaf: a named file with a byte size.
struct File {
    std::string name;
    std::size_t size;
};

/// Group: a named directory owning child nodes by value.
struct Directory {
    std::string name;
    std::vector<Node> children;
};

/// Convenience factory for a leaf node.
inline Node file(std::string name, std::size_t size) {
    return File{std::move(name), size};
}

/// Convenience factory for a group node.
inline Node directory(std::string name, std::vector<Node> children) {
    return Directory{std::move(name), std::move(children)};
}

/// Total byte size of a subtree, computed by a self-recursive lambda that
/// takes itself via *deducing this* and dispatches with `std::visit`.
inline std::size_t total_size(const Node& root) {
    auto sum = [](this auto&& self, const Node& n) -> std::size_t {
        return std::visit(
            gof::overloaded{
                [](const File& f) -> std::size_t { return f.size; },
                [&self](const Directory& d) -> std::size_t {
                    std::size_t acc = 0;
                    for (const Node& child : d.children) acc += self(child);
                    return acc;
                }},
            n);
    };
    return sum(root);
}

/// Count the number of leaf files in a subtree (also via deducing this).
inline std::size_t count_files(const Node& root) {
    auto counter = [](this auto&& self, const Node& n) -> std::size_t {
        return std::visit(
            gof::overloaded{
                [](const File&) -> std::size_t { return 1; },
                [&self](const Directory& d) -> std::size_t {
                    std::size_t acc = 0;
                    for (const Node& child : d.children) acc += self(child);
                    return acc;
                }},
            n);
    };
    return counter(root);
}

/// Render the tree as an indented listing (one entry per line).
inline std::string render(const Node& root) {
    std::string out;
    auto draw = [&out](this auto&& self, const Node& n, int depth) -> void {
        std::string indent(static_cast<std::size_t>(depth) * 2, ' ');
        std::visit(
            gof::overloaded{
                [&](const File& f) {
                    out += indent + f.name + " (" + std::to_string(f.size) +
                           ")\n";
                },
                [&](const Directory& d) {
                    out += indent + d.name + "/\n";
                    for (const Node& child : d.children) self(child, depth + 1);
                }},
            n);
    };
    draw(root, 0);
    return out;
}

}  // namespace mcpp::structural26

#endif  // MCPP_STRUCTURAL26_COMPOSITE_HPP
