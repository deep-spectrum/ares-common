cmake_minimum_required(VERSION 3.14)
project(workq_tests)

set(CMAKE_CXX_STANDARD 20)
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

include_directories(../../include include)

if (RT_TESTS)
    message(STATUS "Adding realtime tests")
    message(WARNING "This requires realtime threading to be enabled. To check if you have realtime threads, run `ulimit -Hr` and `ulimit -Sr` and make sure you have a sufficient amount of realtime threads available. If not, the realtime tests will be skipped...")
else ()
    message(STATUS "Skipping realtime tests")
    add_definitions(-DSKIP_RT_TESTS)
endif ()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../test-helpers test-helpers)

set(WORK_Q_SRC
        ../../src/work-q/work_q.cpp
)

################################################
# Add test executables here
################################################

add_executable(
        task_test
        src/task_tests.cpp
)
target_link_libraries(task_test GTest::gtest_main)

#add_executable(
#        work_queue_test
#        src/work_queue_tests.cpp ${WORK_Q_SRC}
#)
#target_compile_definitions(work_queue_test PRIVATE SYS_WORK_QUEUE=1)
#target_link_libraries(work_queue_test GTest::gtest_main)

add_executable(
        work_test
        src/work_tests.cpp ${WORK_Q_SRC}
)
target_compile_definitions(work_test PRIVATE SYS_WORK_QUEUE=1)
target_link_libraries(work_test GTest::gtest_main ares_test_helpers)

################################################
# Discover tests here
################################################
include(GoogleTest)
gtest_discover_tests(task_test)
# gtest_discover_tests(work_queue_test)
gtest_discover_tests(work_test)
