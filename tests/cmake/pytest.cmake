cmake_policy(PUSH)

function(discover_pytest_tests)

    cmake_parse_arguments(
            ""
            ""
            "INTERPRETER;WORKING_DIRECTORY;TEST_PREFIX;TEST_SUFFIX"
            "EXTRA_ARGS;EXECUTION_ARGS;COLLECTION_ARGS;PACKAGE_DIR"
            ${ARGN}
    )

    if (NOT _WORKING_DIRECTORY)
        set(_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif ()

    if (NOT _TEST_PREFIX)
        set(_TEST_PREFIX "")
    endif ()

    if (NOT _TEST_SUFFIX)
        set(_TEST_SUFFIX "")
    endif ()

    if (NOT _PACKAGE_DIR)
        set(_PACKAGE_DIR "${_WORKING_DIRECTORY}")
    endif ()

    set(pytest_collect_args --collect-only -q ${_EXTRA_ARGS} ${_COLLECTION_ARGS})
    set(pytest_execution_args ${_EXTRA_ARGS} ${_EXECUTION_ARGS})
    set(pytest_base_command ${_INTERPRETER} -m pytest)

    execute_process(
            COMMAND ${pytest_base_command} ${pytest_collect_args}
            WORKING_DIRECTORY ${_WORKING_DIRECTORY}
            OUTPUT_VARIABLE pytest_collect_output
            ERROR_VARIABLE pytest_collect_error
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE pytest_collect_result
    )

    if (NOT ${pytest_collect_result} EQUAL 0)
        message(FATAL_ERROR "Pytest discovery failed!\n"
                "Expected working directory: ${_WORKING_DIRECTORY}\n"
                "Exit Code: ${pytest_collect_result}\n"
                "STDOUT:\n${pytest_collect_output}\n"
                "STDERR:\n${pytest_collect_error}")
    endif ()

    string(REPLACE [[;]] [[\;]] pytest_collect_output "${pytest_collect_output}")
    string(REPLACE "\n" ";" pytest_collect_output "${pytest_collect_output}")

    set(pytest_testcase_regex "([^:]+)(::([^:]+))?::([^:]+)")

    foreach (test_case ${pytest_collect_output})
        string(REGEX MATCHALL ${pytest_testcase_regex} test_case_match "${test_case}")

        if(test_case_match)
            set(test_case_file ${CMAKE_MATCH_1})
            set(test_case_class ${CMAKE_MATCH_3})
            set(test_case_func ${CMAKE_MATCH_4})

            if(test_case_class)
                set(test_case_name "${_TEST_PREFIX}${test_case_class}.${test_case_func}${_TEST_SUFFIX}")
            else()
                set(test_case_name "${_TEST_PREFIX}${test_case_func}${_TEST_SUFFIX}")
            endif()

            add_test(
                    NAME "${test_case_name}"
                    COMMAND ${pytest_base_command} ${test_case} ${pytest_execution_args} ${pytest_local_args}
                    WORKING_DIRECTORY ${_PACKAGE_DIR}
            )
            set_tests_properties("${test_case_name}" PROPERTIES SKIP_RETURN_CODE 5)
            message(STATUS "Registered ${test_case_name}")
        endif ()
    endforeach ()
endfunction()

cmake_policy(POP)