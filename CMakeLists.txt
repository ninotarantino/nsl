cmake_minimum_required(VERSION 3.25)

project(nsl
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "A library of miscellaneous tools"
    HOMEPAGE_URL "https://github.com/ninotarantino/nsl"
)
set(CPACK_PACKAGE_VENDOR "Nino Tarantino")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(nsl INTERFACE)
target_include_directories(
    nsl INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  zip_test
  test/ranges/zip_test.cc
)
target_link_libraries(
  zip_test
  GTest::gmock_main
  GTest::gtest_main
  nsl
)

add_executable(
  polynomial_test
  test/math/polynomial_test.cc
)
target_link_libraries(
  polynomial_test
  GTest::gmock_main
  GTest::gtest_main
  nsl
)

include(GoogleTest)
gtest_discover_tests(zip_test)
gtest_discover_tests(polynomial_test)

