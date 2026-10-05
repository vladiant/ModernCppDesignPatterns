cmake_minimum_required(VERSION 3.28)
project(ModernCppDesignPatterns LANGUAGES CXX)

# Optional aggregate build: wires all 8 standalone pattern projects into one
# tree for convenience/CI. Each subproject remains independently buildable from
# its own directory. This root fetches Catch2 ONCE; subprojects reuse the target
# via their own `if(NOT TARGET Catch2::Catch2WithMain)` guard (DESIGN DD-2).

include(CTest) # enable_testing() for the whole tree + defines BUILD_TESTING

if(BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
    GIT_SHALLOW    TRUE
  )
  FetchContent_MakeAvailable(Catch2)
  list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
endif()

# ---- subproject selection ----------------------------------------------------
# PATTERN_CXX20_ONLY lets a portability job (e.g. CI's clang-18 build) aggregate
# ONLY the four C++20 subprojects. The four C++23 subprojects require GCC 14
# (deducing-this / std::expected), which clang-18 + libstdc++ 13 cannot provide.
# Default OFF => all 8 projects are built exactly as before, so this option is a
# pure build-set filter: it changes nothing about any subproject's sources, its
# language standard, or its public interface.
option(PATTERN_CXX20_ONLY
       "Aggregate only the C++20 subprojects (portability builds without full C++23 support)"
       OFF)

# C++20 subprojects — build on g++-13, g++-14 and clang-18.
add_subdirectory(patterns/creational/abstract-factory-cpp20)
add_subdirectory(patterns/structural/adapter-cpp20)
add_subdirectory(patterns/behavioral/strategy-cpp20)
add_subdirectory(patterns/behavioral/observer-cpp20)

# C++23 subprojects — require GCC 14; skipped when PATTERN_CXX20_ONLY is ON.
if(NOT PATTERN_CXX20_ONLY)
  add_subdirectory(patterns/creational/builder-cpp23)
  add_subdirectory(patterns/structural/decorator-cpp23)
  add_subdirectory(patterns/behavioral/command-cpp23)
  add_subdirectory(patterns/behavioral/visitor-cpp23)
endif()
