if(NOT DEFINED BRANCHTALK_LOGGING_SMOKE)
    message(FATAL_ERROR "BRANCHTALK_LOGGING_SMOKE is required")
endif()

execute_process(
    COMMAND "${BRANCHTALK_LOGGING_SMOKE}"
    RESULT_VARIABLE logging_result
    OUTPUT_VARIABLE logging_output
    ERROR_VARIABLE logging_error
)

if(NOT logging_result EQUAL 0)
    message(FATAL_ERROR "Logging smoke test failed with exit code ${logging_result}")
endif()

string(APPEND logging_output "\n${logging_error}")

foreach(expected_output IN ITEMS
        "[client] client category ready"
        "[server] server category ready"
        "[database] database category ready"
        "[network] network category ready"
        "[database] credential=<redacted>")
    string(FIND "${logging_output}" "${expected_output}" output_index)
    if(output_index EQUAL -1)
        message(FATAL_ERROR "Missing structured log output: ${expected_output}")
    endif()
endforeach()

string(FIND "${logging_output}" "database-secret" secret_index)
if(NOT secret_index EQUAL -1)
    message(FATAL_ERROR "Sensitive value appeared in log output")
endif()