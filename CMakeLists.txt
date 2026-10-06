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

# ---- C++26 tier (requires g++-14 -std=c++26) --------------------------------
# Default OFF so the existing C++20 (all compilers) / C++23 (g++-14) aggregate
# is unchanged on non-g++-14 jobs: with PATTERN_ENABLE_CPP26 OFF the aggregate is
# byte-for-byte behaviour-identical to before. CI's dedicated g++-14 + CMake>=3.30
# job (build-cpp26) passes -DPATTERN_ENABLE_CPP26=ON. Orthogonal to
# PATTERN_CXX20_ONLY (DESIGN §C26.7).
option(PATTERN_ENABLE_CPP26
       "Aggregate the C++26 idiom tier (requires g++-14 -std=c++26)" OFF)

# The reflection showcase is independently gated (DESIGN §C26.5). It builds on no
# compiler available here, so it is NEVER added by default and NEVER in CI: it is
# wired in only when PATTERN_ENABLE_REFLECTION is explicitly ON (and also only
# while the C++26 tier is enabled). CI never sets it → CI stays green.
option(PATTERN_ENABLE_REFLECTION
       "Build the C++26 reflection showcase (needs an experimental P2996 compiler)" OFF)

if(PATTERN_ENABLE_CPP26)
  add_subdirectory(patterns/creational/creational-cpp26)
  add_subdirectory(patterns/structural/structural-cpp26)
  add_subdirectory(patterns/behavioral/behavioral-cpp26)
  if(PATTERN_ENABLE_REFLECTION)
    add_subdirectory(patterns/reflection/reflection-cpp26)
  endif()
endif()
