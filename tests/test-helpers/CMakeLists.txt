cmake_minimum_required(VERSION 3.14)
project(test_helpers)

set(SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/thread_utils.cpp
)

if(NOT TARGET ares_test_helpers)
    add_library(ares_test_helpers SHARED)
    target_sources(ares_test_helpers PRIVATE ${SOURCES})
    target_include_directories(ares_test_helpers PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
endif()
