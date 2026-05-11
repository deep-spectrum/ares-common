cmake_minimum_required(VERSION 3.14)
project(data_structure4_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
        ${CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES}
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include/common"
        CACHE INTERNAL "Submodule Global Headers")

enable_testing()

include_directories(../../include include)

set(LOG_LIB_SRC
        ../../src/logging/logger.cpp
)

################################################
# Add test executables here
################################################

add_executable(
        logger_test
        src/test_logger.cpp
        ${LOG_LIB_SRC}
)
target_link_libraries(logger_test GTest::gtest_main)

################################################
# Discover tests here
################################################

include(GoogleTest)
gtest_discover_tests(logger_test)
