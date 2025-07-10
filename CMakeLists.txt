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
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  zip_test
  test/ranges/zip_test.cc
)
target_link_libraries(
  zip_test
  GTest::gtest_main
  nsl
)

include(GoogleTest)
gtest_discover_tests(zip_test)

