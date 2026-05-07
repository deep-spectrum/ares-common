cmake_minimum_required(VERSION 3.14)
project(data_structure4_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The idiots of the C++ standard didn't think about thread priority when
# implementing conditional variables. Fucking idiots...
option(RT_TESTS "Add realtime tests" OFF)

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

include_directories(../../include)

if (RT_TESTS)
    message(STATUS "Adding realtime tests")
    message(WARNING "This requires realtime threading to be enabled. To check if you have realtime threads, run `ulimit -Hr` and `ulimit -Sr` and make sure you have a sufficient amount of realtime threads available. If not, the realtime tests will be skipped...")
else ()
    message(STATUS "Skipping realtime tests")
    add_definitions(-DSKIP_RT_TESTS)
endif ()

################################################
# Add test executables here
################################################

add_executable(
        slist_test
        src/slist_test.cpp
)
target_link_libraries(slist_test GTest::gtest_main)

add_executable(
        queue_test
        src/queue_tests.cpp
)
target_link_libraries(queue_test GTest::gtest_main)

################################################
# Discover tests here
################################################

include(GoogleTest)
gtest_discover_tests(slist_test)
gtest_discover_tests(queue_test)
