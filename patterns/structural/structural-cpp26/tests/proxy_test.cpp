/// \file proxy_test.cpp
/// \brief Catch2 tests for Proxy (C12): a virtual proxy loads its expensive
///        real subject exactly once via std::call_once + std::once_flag.

#include <mcpp/structural26/proxy.hpp>

#include <catch2/catch_test_macros.hpp>

#include <thread>
#include <vector>

using namespace mcpp::structural26;

TEST_CASE("Proxy defers construction until first use", "[proxy]") {
    RealImage::reset_load_count();
    ImageProxy image{"photo.png"};
    REQUIRE_FALSE(image.is_loaded());
    REQUIRE(RealImage::load_count() == 0);

    REQUIRE(image.draw() == "<image photo.png>");
    REQUIRE(image.is_loaded());
    REQUIRE(RealImage::load_count() == 1);
}

TEST_CASE("Repeated draws load the real subject only once", "[proxy]") {
    RealImage::reset_load_count();
    ImageProxy image{"a.png"};
    for (int i = 0; i < 5; ++i) REQUIRE(image.draw() == "<image a.png>");
    REQUIRE(RealImage::load_count() == 1);
}

TEST_CASE("Concurrent first-use loads the subject exactly once",
          "[proxy][edge]") {
    RealImage::reset_load_count();
    ImageProxy image{"shared.png"};

    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i)
        threads.emplace_back([&image] {
            for (int j = 0; j < 100; ++j) (void)image.draw();
        });
    for (auto& t : threads) t.join();

    REQUIRE(image.is_loaded());
    REQUIRE(RealImage::load_count() == 1);
}
