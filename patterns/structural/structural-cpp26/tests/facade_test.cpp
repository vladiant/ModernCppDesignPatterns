/// \file facade_test.cpp
/// \brief Catch2 tests for Facade (C10): subsystems chained with std::expected
///        monadic operations; errors short-circuit the pipeline.

#include <mcpp/structural26/facade.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace mcpp::structural26;

TEST_CASE("A valid order flows through all three subsystems", "[facade]") {
    OrderFacade facade;
    auto result = facade.place(Order{"book", 2, 100.0});
    REQUIRE(result.has_value());
    REQUIRE(result->tracking == "TRK-book");
    REQUIRE(result->amount_charged == 2 * 9.99);
}

TEST_CASE("An empty cart short-circuits at validate", "[facade][edge]") {
    OrderFacade facade;
    auto result = facade.place(Order{"book", 0, 100.0});
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == OrderError::empty_cart);
}

TEST_CASE("Too-large quantity is out of stock", "[facade][edge]") {
    OrderFacade facade;
    auto result = facade.place(Order{"book", 50, 10000.0});
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == OrderError::out_of_stock);
}

TEST_CASE("Insufficient funds fail at charge", "[facade]") {
    OrderFacade facade;
    auto result = facade.place(Order{"book", 3, 5.0});
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == OrderError::insufficient_funds);
}

TEST_CASE("Missing item name fails at ship", "[facade][edge]") {
    OrderFacade facade;
    auto result = facade.place(Order{"", 1, 100.0});
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == OrderError::no_carrier);
}

TEST_CASE("transform maps a success to a receipt string", "[facade]") {
    OrderFacade facade;
    auto receipt = facade.place_receipt(Order{"book", 1, 100.0});
    REQUIRE(receipt.has_value());
    REQUIRE(receipt->rfind("shipped TRK-book", 0) == 0);
}

TEST_CASE("transform preserves the error channel", "[facade]") {
    OrderFacade facade;
    auto receipt = facade.place_receipt(Order{"book", 0, 100.0});
    REQUIRE_FALSE(receipt.has_value());
    REQUIRE(receipt.error() == OrderError::empty_cart);
}
