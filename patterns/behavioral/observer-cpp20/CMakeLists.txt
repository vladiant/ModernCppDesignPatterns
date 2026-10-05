cmake_minimum_required(VERSION 3.28)
project(observer_cpp20 LANGUAGES CXX)

# ---- standard enforcement (NFR-4) ----
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- warnings policy (NFR-3) ----
option(PATTERN_WERROR "Treat compiler warnings as errors" OFF)

if(NOT TARGET pattern_warnings)
  add_library(pattern_warnings INTERFACE)
  target_compile_options(pattern_warnings INTERFACE
    -Wall -Wextra -Wpedantic
    $<$<BOOL:${PATTERN_WERROR}>:-Werror>)
endif()

# ---- the pattern code: header-only INTERFACE library ----
add_library(observer INTERFACE)
target_include_directories(observer INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(observer INTERFACE cxx_std_20)

# ---- demo executable (FR-2) ----
add_executable(observer_demo app/main.cpp)
target_link_libraries(observer_demo
  PRIVATE observer pattern_warnings)

# ---- tests (FR-3) : only when testing is on ----
include(CTest)
if(BUILD_TESTING)
  # --- Catch2 v3: reuse aggregate's copy if present, else find, else fetch ---
  if(NOT TARGET Catch2::Catch2WithMain)
    find_package(Catch2 3 QUIET)
  endif()
  if(NOT TARGET Catch2::Catch2WithMain)
    include(FetchContent)
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG        v3.7.1
      GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(Catch2)
  endif()

  if(DEFINED catch2_SOURCE_DIR)
    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
  endif()
  include(Catch)

  add_executable(observer_tests tests/observer_test.cpp)
  target_link_libraries(observer_tests
    PRIVATE observer pattern_warnings Catch2::Catch2WithMain)

  catch_discover_tests(observer_tests)
endif()
