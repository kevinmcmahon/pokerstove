if(NOT DEFINED PS_EVAL)
    message(FATAL_ERROR "PS_EVAL must be set")
endif()

if(NOT DEFINED EXPECTED_REGEX)
    message(FATAL_ERROR "EXPECTED_REGEX must be set")
endif()

execute_process(
    COMMAND "${PS_EVAL}" ${TEST_ARGS}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error)

set(combined_output "${output}${error}")

if(result EQUAL 0)
    message(FATAL_ERROR "Expected command failure but it succeeded:\n${combined_output}")
endif()

if(NOT combined_output MATCHES "${EXPECTED_REGEX}")
    message(FATAL_ERROR
        "Expected output matching '${EXPECTED_REGEX}', got:\n${combined_output}")
endif()
