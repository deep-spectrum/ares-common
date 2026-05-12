cmake_minimum_required(VERSION 3.14)
project(ares_tests)

add_subdirectory(ares-common/internals)
add_subdirectory(ares-data-structures)
add_subdirectory(ares-logging)
add_subdirectory(ares-synchronization)

enable_testing()

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(VENV_DIR "${CMAKE_BINARY_DIR}/venv")

if (NOT EXISTS "${VENV_DIR}")
    message(STATUS "Creating Python environment")
    execute_process(
            COMMAND "${Python3_EXECUTABLE}" -m venv "${VENV_DIR}"
    )
endif ()

set(VENV_PYTHON "${VENV_DIR}/bin/python")

execute_process(
        COMMAND "${VENV_PYTHON}" -m pip install "${CMAKE_CURRENT_SOURCE_DIR}/ares-common/pyutils"
)

add_test(
        NAME pyutils-tests
        COMMAND "${VENV_PYTHON}" -m pytest "${CMAKE_CURRENT_SOURCE_DIR}/ares-common/pyutils/tests"
)
