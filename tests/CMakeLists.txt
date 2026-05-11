cmake_minimum_required(VERSION 3.14)
project(ares_tests)

add_subdirectory(ares-common/internals)
add_subdirectory(ares-data-structures)
add_subdirectory(ares-logging)
add_subdirectory(ares-synchronization)

enable_testing()
