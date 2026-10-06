/// \file chain_test.cpp
/// \brief Catch2 tests for Chain of Responsibility (C20): first engaged
///        optional-returning handler wins.

#include <mcpp/behavioral26/chain.hpp>

#include <catch2/catch_test_macros.hpp>

#include <optional>
#include <string>

using namespace mcpp::behavioral26;

namespace {
ApprovalChain make_chain() {
    ApprovalChain chain;
    chain
        .then([](const Request& r) -> std::optional<std::string> {
            if (r.amount <= 100)
                return "team lead approved " + std::to_string(r.amount);
            return std::nullopt;
        })
        .then([](const Request& r) -> std::optional<std::string> {
            if (r.amount <= 1000)
                return "manager approved " + std::to_string(r.amount);
            return std::nullopt;
        });
    return chain;
}
}  // namespace

TEST_CASE("the first engaged tier approves", "[chain]") {
    auto chain = make_chain();
    REQUIRE(chain.handle({"alice", 50}) == "team lead approved 50");
    REQUIRE(chain.handle({"alice", 500}) == "manager approved 500");
}

TEST_CASE("boundary amounts resolve to the lower tier", "[chain][edge]") {
    auto chain = make_chain();
    REQUIRE(chain.handle({"alice", 100}) == "team lead approved 100");
    REQUIRE(chain.handle({"alice", 1000}) == "manager approved 1000");
}

TEST_CASE("nobody approves yields a rejection", "[chain][edge]") {
    auto chain = make_chain();
    REQUIRE(chain.handle({"alice", 5000}) ==
            "rejected: nobody can approve 5000");
}

TEST_CASE("an empty chain always rejects", "[chain][edge]") {
    ApprovalChain chain;
    REQUIRE(chain.handle({"bob", 1}) == "rejected: nobody can approve 1");
}
