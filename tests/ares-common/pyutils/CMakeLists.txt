cmake_minimum_required(VERSION 3.15)
project(${SKBUILD_PROJECT_NAME} VERSION ${SKBUILD_PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ version selection")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_BRANCH stable
)
FetchContent_MakeAvailable(pybind11)
find_package(Python COMPONENTS Interpreter Development REQUIRED)

list(APPEND "${CMAKE_CURRENT_SOURCE_DIR}../../../cmake")

set(PACKAGE_NAME pyutils_test_core)
python_package(src/${PACKAGE_NAME} DESTINATION ${PACKAGE_NAME})
python_module(_pyutils_test
        src/pyutils_test/test_array_to_tuple.cpp
        src/pyutils_test/test_from_kwargs.cpp
        src/pyutils_test/test_to_dict.cpp
        DESTINATION ${PACKAGE_NAME})
