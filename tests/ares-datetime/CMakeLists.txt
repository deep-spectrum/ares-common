cmake_minimum_required(VERSION 3.15)
project(${SKBUILD_PROJECT_NAME} VERSION ${SKBUILD_PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ version selection")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG stable
)
FetchContent_MakeAvailable(pybind11)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")

include (AddPythonModule)

include_directories(include ../../include)

set(PACKAGE_NAME datetime_test_core)
python_package(src/python/${PACKAGE_NAME} DESTINATION ${PACKAGE_NAME})
python_module(_datetime_test
        src/python/datetime_test/bindings.cpp
        ../../src/datetime/datetime.cpp
        DESTINATION ${PACKAGE_NAME})