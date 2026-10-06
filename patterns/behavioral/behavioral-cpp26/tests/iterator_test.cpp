/// \file iterator_test.cpp
/// \brief Catch2 tests for Iterator (C19): in-order traversal yields {1..7}.

#include <mcpp/behavioral26/iterator.hpp>

#include <catch2/catch_test_macros.hpp>

#include <vector>

using namespace mcpp::behavioral26;

namespace {
/// Collect an in-order traversal into a vector, using whichever traversal form
/// the toolchain provides (std::generator on g++-14, else function_ref).
std::vector<int> collect_inorder(const TreeNode* root) {
    std::vector<int> out;
#if defined(__cpp_lib_generator)
    for (int x : inorder(root)) out.push_back(x);
#else
    inorder(root, [&out](int x) { out.push_back(x); });
#endif
    return out;
}
}  // namespace

TEST_CASE("in-order traversal of a balanced tree yields 1..7", "[iterator]") {
    auto tree = node(4, node(2, leaf(1), leaf(3)), node(6, leaf(5), leaf(7)));
    REQUIRE(collect_inorder(tree.get()) ==
            std::vector<int>{1, 2, 3, 4, 5, 6, 7});
}

TEST_CASE("a single leaf yields one element", "[iterator][edge]") {
    auto tree = leaf(42);
    REQUIRE(collect_inorder(tree.get()) == std::vector<int>{42});
}

TEST_CASE("an empty (null) tree yields nothing", "[iterator][edge]") {
    REQUIRE(collect_inorder(nullptr).empty());
}
