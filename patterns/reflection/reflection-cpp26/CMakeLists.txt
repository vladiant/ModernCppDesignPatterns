cmake_minimum_required(VERSION 3.30)
project(reflection_cpp26 LANGUAGES CXX)

# ---- standard enforcement (NFR-C26-6) ----
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- reflection feature probe (FR-C26-8, DD-C26-8, C26.5) --------------------
# P2996 static reflection (`^^T`, `[: :]`, `template for`) builds on NO compiler
# available here (g++-14 / g++-13 / clang-18 all lack __cpp_reflection). This
# standalone-layer gate probes the compiler and, when the feature is absent,
# CONFIGURES SUCCESSFULLY BUT BUILDS NOTHING — printing a clear SKIP status so a
# direct `cmake -S . -B build` is a graceful no-op instead of a hard error.
# (The aggregate root additionally never add_subdirectory()s this project unless
#  PATTERN_ENABLE_REFLECTION=ON, which CI never sets — AC-C26-6/7, NFR-C26-4.)
include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_FLAGS "-std=c++26")
check_cxx_source_compiles("
  #if !defined(__cpp_reflection)
  #error no reflection
  #endif
  int main() {}
" HAVE_CPP26_REFLECTION)

if(NOT HAVE_CPP26_REFLECTION)
  message(STATUS
    "reflection_cpp26: no __cpp_reflection on this compiler — SKIPPING build "
    "(source is present; needs an experimental P2996 compiler). CI stays green.")
  return()                          # configure succeeds, builds nothing
endif()

# ======= Below is only reached on a reflection-capable compiler. =============

# ---- warnings policy (NFR-C26-5) : identical PATTERN_WERROR carrier as other tiers ----
option(PATTERN_WERROR "Treat compiler warnings as errors" OFF)
if(NOT TARGET pattern_warnings)
  add_library(pattern_warnings INTERFACE)
  target_compile_options(pattern_warnings INTERFACE
    -Wall -Wextra -Wpedantic
    $<$<BOOL:${PATTERN_WERROR}>:-Werror>)
endif()

# ---- pattern code: header-only INTERFACE lib ----
add_library(reflection26 INTERFACE)
target_include_directories(reflection26 INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(reflection26 INTERFACE cxx_std_26)

# ---- demo (FR-C26-3) ----
add_executable(reflection26_demo app/main.cpp)
target_link_libraries(reflection26_demo PRIVATE reflection26 pattern_warnings)

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

  add_executable(reflection26_tests tests/reflect_test.cpp)
  target_link_libraries(reflection26_tests
    PRIVATE reflection26 pattern_warnings Catch2::Catch2WithMain)
  catch_discover_tests(reflection26_tests)
endif()
