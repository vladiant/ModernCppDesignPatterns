cmake_minimum_required(VERSION 3.30)
project(structural_cpp26 LANGUAGES CXX)

# ---- standard enforcement (NFR-C26-6) ----
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- warnings policy (NFR-C26-5) : identical PATTERN_WERROR carrier as other tiers ----
option(PATTERN_WERROR "Treat compiler warnings as errors" OFF)
if(NOT TARGET pattern_warnings)
  add_library(pattern_warnings INTERFACE)
  target_compile_options(pattern_warnings INTERFACE
    -Wall -Wextra -Wpedantic
    $<$<BOOL:${PATTERN_WERROR}>:-Werror>)
endif()

# ---- pattern code: header-only INTERFACE lib (compat.hpp travels with it) ----
add_library(structural26 INTERFACE)
target_include_directories(structural26 INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(structural26 INTERFACE cxx_std_26)

# ---- demo (FR-C26-3) ----
add_executable(structural26_demo app/main.cpp)
target_link_libraries(structural26_demo PRIVATE structural26 pattern_warnings)

# ---- tests (FR-C26-4) ----
include(CTest)
if(BUILD_TESTING)
  if(NOT TARGET Catch2::Catch2WithMain)
    find_package(Catch2 3 QUIET)
  endif()
  if(NOT TARGET Catch2::Catch2WithMain)
    include(FetchContent)
    FetchContent_Declare(Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG v3.7.1 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(Catch2)
  endif()
  if(DEFINED catch2_SOURCE_DIR)
    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
  endif()
  include(Catch)

  add_executable(structural26_tests
    tests/adapter_test.cpp tests/bridge_test.cpp tests/composite_test.cpp
    tests/decorator_test.cpp tests/facade_test.cpp tests/flyweight_test.cpp
    tests/proxy_test.cpp)
  target_link_libraries(structural26_tests
    PRIVATE structural26 pattern_warnings Catch2::Catch2WithMain)
  catch_discover_tests(structural26_tests)
endif()
