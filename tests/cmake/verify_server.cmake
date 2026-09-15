foreach(required_variable IN ITEMS BRANCHTALK_SERVER BRANCHTALK_INVALID_CONFIG)
    if(NOT DEFINED ${required_variable})
        message(FATAL_ERROR "${required_variable} is required")
    endif()
endforeach()

execute_process(
    COMMAND "${BRANCHTALK_SERVER}" --smoke-test
    RESULT_VARIABLE lifecycle_result
    OUTPUT_VARIABLE lifecycle_stdout
    ERROR_VARIABLE lifecycle_stderr
)

if(NOT lifecycle_result EQUAL 0)
    message(FATAL_ERROR
        "Server lifecycle failed with ${lifecycle_result}:\n${lifecycle_stdout}\n${lifecycle_stderr}")
endif()

set(lifecycle_output "${lifecycle_stdout}\n${lifecycle_stderr}")
foreach(expected_message IN ITEMS "[server] server started" "[server] server stopped")
    string(FIND "${lifecycle_output}" "${expected_message}" message_index)
    if(message_index EQUAL -1)
        message(FATAL_ERROR "Missing server lifecycle output: ${expected_message}")
    endif()
endforeach()

execute_process(
    COMMAND "${BRANCHTALK_SERVER}" "${BRANCHTALK_INVALID_CONFIG}"
    RESULT_VARIABLE invalid_config_result
    OUTPUT_VARIABLE invalid_config_stdout
    ERROR_VARIABLE invalid_config_stderr
)

if(NOT invalid_config_result EQUAL 1)
    message(FATAL_ERROR
        "Invalid configuration returned ${invalid_config_result}, expeceted 1:\n"
        "${invalid_config_stdout}\n${invalid_config_stderr}")
endif()

set(invalid_config_output "${invalid_config_stdout}\n${invalid_config_stderr}")
foreach(expected_message IN ITEMS
    "[error] [server] server startup error"
    "log_level"
)
    string(FIND "${invalid_config_output}" "${expected_message}" message_index)
    if(message_index EQUAL -1)
        message(FATAL_ERROR "Missing invalid configuration output: ${expected_message}")
    endif()
endforeach()