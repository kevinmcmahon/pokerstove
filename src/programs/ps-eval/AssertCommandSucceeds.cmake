if(NOT DEFINED PS_EVAL)
    message(FATAL_ERROR "PS_EVAL must be set")
endif()

execute_process(
    COMMAND "${PS_EVAL}" ${TEST_ARGS}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error)

set(combined_output "${output}${error}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR "Expected command success but it failed:\n${combined_output}")
endif()

foreach(regex IN LISTS EXPECTED_REGEXES)
    if(NOT combined_output MATCHES "${regex}")
        message(FATAL_ERROR
            "Expected output matching '${regex}', got:\n${combined_output}")
    endif()
endforeach()

foreach(regex IN LISTS FORBIDDEN_REGEXES)
    if(combined_output MATCHES "${regex}")
        message(FATAL_ERROR
            "Output unexpectedly matched '${regex}':\n${combined_output}")
    endif()
endforeach()
