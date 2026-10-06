/// \file iterator.hpp
/// \brief Iterator (C19) — in-order binary-tree traversal as `std::generator`
///        when `__cpp_lib_generator` is available (the case on g++-14), else an
///        internal iterator driven by `gof::function_ref<void(int)>`.

#ifndef MCPP_BEHAVIORAL26_ITERATOR_HPP
#define MCPP_BEHAVIORAL26_ITERATOR_HPP

#include "compat.hpp"

#include <memory>
#include <utility>
#include <version>  // exposes __cpp_lib_generator before we probe it

#if defined(__cpp_lib_generator)
#include <generator>
#include <ranges>
#endif

namespace mcpp::behavioral26 {

/// A binary tree node owning its children.
struct TreeNode {
    int value;
    std::unique_ptr<TreeNode> left, right;
};

/// Build a leaf node.
inline std::unique_ptr<TreeNode> leaf(int v) {
    return std::make_unique<TreeNode>(v);
}

/// Build an internal node with two children.
inline std::unique_ptr<TreeNode> node(int v, std::unique_ptr<TreeNode> l,
                                      std::unique_ptr<TreeNode> r) {
    return std::make_unique<TreeNode>(v, std::move(l), std::move(r));
}

#if defined(__cpp_lib_generator)
/// Lazily yield the tree's values in-order as a coroutine range.
inline std::generator<int> inorder(const TreeNode* n) {
    if (!n) co_return;
    co_yield std::ranges::elements_of(inorder(n->left.get()));
    co_yield n->value;
    co_yield std::ranges::elements_of(inorder(n->right.get()));
}
#else
/// Internal-iterator fallback: visit each value in-order via a non-owning
/// `gof::function_ref<void(int)>` (DD-C26-11).
inline void inorder(const TreeNode* n, gof::function_ref<void(int)> visit) {
    if (!n) return;
    inorder(n->left.get(), visit);
    visit(n->value);
    inorder(n->right.get(), visit);
}
#endif

}  // namespace mcpp::behavioral26

#endif  // MCPP_BEHAVIORAL26_ITERATOR_HPP
