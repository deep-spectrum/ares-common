cmake_minimum_required(VERSION 3.15)
project(ares-common)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
        ${CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES}
        "${CMAKE_CURRENT_SOURCE_DIR}/include/common"
        CACHE INTERNAL "Submodule Global Headers")

option(TESTS "Include unit tests in the build" OFF)

# The idiots of the C++ standard didn't think about thread priority when
# implementing conditional variables. Fucking idiots...
option(RT_TESTS "Add realtime tests" OFF)

###########################################
# Logging
###########################################

option(USE_PYTHON_LOGGERS "Use Python logging library implementation" OFF)

set(DEFAULT_LOG_LEVEL "off" CACHE STRING "Logging level: off, crit, err, wrn, inf, dbg")
set_property(CACHE DEFAULT_LOG_LEVEL PROPERTY STRINGS off err wrn inf dbg)

if (DEFAULT_LOG_LEVEL STREQUAL "off")
    set(LOG_LEVEL_VAL 5)
elseif(DEFAULT_LOG_LEVEL STREQUAL "crit")
    set(LOG_LEVEL_VAL 4)
elseif (DEFAULT_LOG_LEVEL STREQUAL "err")
    set(LOG_LEVEL_VAL 3)
elseif (DEFAULT_LOG_LEVEL STREQUAL "wrn")
    set(LOG_LEVEL_VAL 2)
elseif (DEFAULT_LOG_LEVEL STREQUAL "inf")
    set(LOG_LEVEL_VAL 1)
elseif (DEFAULT_LOG_LEVEL STREQUAL "dbg")
    set(LOG_LEVEL_VAL 0)
else ()
    message(WARNING "Log level not set. Selecting inf level")
    set(LOG_LEVEL_VAL 1)
endif ()

add_library(logging STATIC src/logging/logger.cpp)
set_target_properties(logging PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(logging PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(logging PUBLIC LOG_LEVEL=${LOG_LEVEL_VAL})

if (USE_PYTHON_LOGGERS)
    find_package(pybind11 QUIET)

    if (TARGET pybind11::embed)
        message(STATUS "Found pybind11::embed")
        target_link_libraries(logging PRIVATE pybind11::embed)
        target_compile_definitions(logging PRIVATE USE_PYTHON_LOGGERS)
    else()
        message(WARNING "USE_PYTHON_LIB is ON but pybind11::embed is not available")
    endif()
endif ()

###########################################
# Serial
###########################################
set(SERIAL_C_API
        src/serial/core/C-API/serial_posix.c
)

set(SERIAL_SRC
        src/serial/core/serial_base.cpp
        src/serial/core/serial_posix.cpp
)

set(SERIAL_TOOLS_CORE
        src/serial/tools/core/sys_fs_common.cpp
        src/serial/tools/core/sys_fs_linux.cpp
)

set(SERIAL_TOOLS
        src/serial/tools/list_ports.cpp
)

add_library(serial STATIC ${SERIAL_C_API} ${SERIAL_SRC} ${SERIAL_TOOLS_CORE} ${SERIAL_TOOLS})
set_target_properties(serial PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(serial PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

###########################################
# Data structures
###########################################

# If sources are needed in the future, switch to STATIC <source files>
add_library(ares_data_structures INTERFACE)
# Uncomment if sources are included in the build
# set_target_properties(ares_data_structures PROPERTIES POSITION_INDEPENDENT_CODE ON)
# If sources are added, switch INTERFACE to PUBLIC
target_include_directories(ares_data_structures INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

###########################################
# Synchronization
###########################################

# If sources are needed in the future, switch to STATIC <source files>
add_library(ares_synchronization INTERFACE)
# Uncomment if sources are included in the build
# set_target_properties(ares_data_structures PROPERTIES POSITION_INDEPENDENT_CODE ON)
# If sources are added, switch INTERFACE to PUBLIC
target_include_directories(ares_synchronization INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

###########################################
# Work queue
###########################################

option(DELAYABLE_WORK "Enable delayable work support" OFF)
option(SYS_WORK_QUEUE "Enable system work queue" OFF)

if (DELAYABLE_WORK)
    message(FATAL_ERROR "Delayable work is not implemented yet")
    set (_DELAYABLE_WORK 1)
else ()
    set (_DELAYABLE_WORK 0)
endif ()

if (SYS_WORK_QUEUE)
    set (_SYS_WORK_QUEUE 1)
else ()
    set (_SYS_WORK_QUEUE 0)
endif ()

add_library(ares_work_q STATIC src/work-q/work_q.cpp)
set_target_properties(ares_work_q PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(ares_work_q PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(ares_work_q PUBLIC SYS_WORK_QUEUE=${_SYS_WORK_QUEUE} DELAYABLE_WORK=${_DELAYABLE_WORK})

###########################################
# Allocators
###########################################

add_library(ares_allocators STATIC
        src/allocators/align_allocator.cpp
        src/allocators/page_allocator.cpp
)
set_target_properties(ares_allocators PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(ares_allocators PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

###########################################
# Mount point
###########################################

add_library(ares_mount_point STATIC
        src/mntpoint/mntpoint.cpp
)
set_target_properties(ares_mount_point PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(ares_mount_point PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

###########################################
# Tests
###########################################

if (TESTS)
    add_subdirectory(tests)
    enable_testing()
endif ()
