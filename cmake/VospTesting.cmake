include_guard(GLOBAL)

include(CMakeParseArguments)

#[=======================================================================[.rst:
vosp_add_compile_fail_test
--------------------------

Adds a CTest case that passes only when the requested target fails to compile.

.. code-block:: cmake

  vosp_add_compile_fail_test(
      NAME invalid_contract
      SOURCES invalid_contract.cpp
      LINK_LIBRARIES vosp::contracts)
#]=======================================================================]
function(vosp_add_compile_fail_test)
    set(options)
    set(one_value_args NAME)
    set(multi_value_args SOURCES LINK_LIBRARIES COMPILE_DEFINITIONS)
    cmake_parse_arguments(VOSP "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(NOT VOSP_NAME)
        message(FATAL_ERROR "vosp_add_compile_fail_test requires NAME")
    endif()
    if(NOT VOSP_SOURCES)
        message(FATAL_ERROR "vosp_add_compile_fail_test requires SOURCES")
    endif()
    if(NOT BUILD_TESTING)
        message(FATAL_ERROR "vosp_add_compile_fail_test requires BUILD_TESTING=ON")
    endif()

    string(MAKE_C_IDENTIFIER "${VOSP_NAME}" test_identifier)
    set(target_name "vosp_compile_fail_${test_identifier}")
    add_executable(${target_name} EXCLUDE_FROM_ALL ${VOSP_SOURCES})
    target_compile_features(${target_name} PRIVATE cxx_std_23)
    if(VOSP_LINK_LIBRARIES)
        target_link_libraries(${target_name} PRIVATE ${VOSP_LINK_LIBRARIES})
    endif()
    if(VOSP_COMPILE_DEFINITIONS)
        target_compile_definitions(${target_name} PRIVATE ${VOSP_COMPILE_DEFINITIONS})
    endif()

    add_test(NAME ${VOSP_NAME}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
                --target ${target_name} --config $<CONFIG>)
    set_tests_properties(${VOSP_NAME} PROPERTIES WILL_FAIL TRUE)
endfunction()
