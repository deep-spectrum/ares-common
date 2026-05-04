cmake_minimum_required(VERSION 3.14)
project(common_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(../../../include/common)

add_executable(
        macro_test
        src/macro_test.cpp
)
target_link_libraries(macro_test GTest::gtest_main)

add_executable(
        utils_test
        src/util_test.cpp
)
target_link_libraries(utils_test GTest::gmock_main)

include(GoogleTest)
gtest_discover_tests(macro_test)
gtest_discover_tests(utils_test)
