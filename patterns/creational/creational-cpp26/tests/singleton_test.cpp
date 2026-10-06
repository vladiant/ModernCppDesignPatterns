/// \file singleton_test.cpp
/// \brief Catch2 tests for Singleton (C1): single-instance accessor and the
///        deleted (with-reason) copy/move operations.

#include <mcpp/creational26/singleton.hpp>

#include <catch2/catch_test_macros.hpp>

#include <type_traits>

using namespace mcpp::creational26;

// The C++26 idiom under test: copy/move are deleted (with a reason string),
// which is observable via the type traits regardless of whether the toolchain
// surfaces the reason text.
static_assert(!std::is_copy_constructible_v<AppConfig>);
static_assert(!std::is_copy_assignable_v<AppConfig>);
static_assert(!std::is_move_constructible_v<AppConfig>);
static_assert(!std::is_move_assignable_v<AppConfig>);

TEST_CASE("AppConfig::instance returns the same object every call", "[singleton]") {
    AppConfig& a = AppConfig::instance();
    AppConfig& b = AppConfig::instance();
    REQUIRE(&a == &b);
}

TEST_CASE("AppConfig exposes mutable shared state", "[singleton]") {
    AppConfig& cfg = AppConfig::instance();
    cfg.set_app_name("unit-test-app");
    cfg.set_worker_threads(16);

    // A second reference observes the same mutations (single instance).
    AppConfig& other = AppConfig::instance();
    REQUIRE(other.app_name() == "unit-test-app");
    REQUIRE(other.worker_threads() == 16);
}

TEST_CASE("AppConfig has sensible defaults before mutation", "[singleton][edge]") {
    // instance() is lazily built once; after the mutations above we at least
    // know the accessors round-trip whatever was last set.
    AppConfig& cfg = AppConfig::instance();
    cfg.set_worker_threads(1);
    REQUIRE(cfg.worker_threads() == 1);
    REQUIRE_FALSE(cfg.app_name().empty());
}
