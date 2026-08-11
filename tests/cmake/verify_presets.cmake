if(NOT DEFINED BRANCHTALK_SOURCE_DIR)
    message(FATAL_ERROR "BRANCHTALK_SOURCE_DIR is required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" --list-presets
    WORKING_DIRECTORY "${BRANCHTALK_SOURCE_DIR}"
    RESULT_VARIABLE preset_result
    OUTPUT_VARIABLE preset_output
    ERROR_VARIABLE preset_error
)

if(NOT preset_result EQUAL 0)
    message(FATAL_ERROR "Could not list CMake presets:\n${preset_error}")
endif()

foreach(expected_preset IN ITEMS debug release)
    string(FIND "${preset_output}" "\"${expected_preset}\"" preset_index)
    if(preset_index EQUAL -1)
        message(FATAL_ERROR "Missing configure preset: ${expected_preset}")
    endif()
endforeach()

file(READ "${BRANCHTALK_SOURCE_DIR}/CMakeLists.txt" project_cmake)

if(project_cmake MATCHES "CMAKE_CXX_FLAGS")
    message(FATAL_ERROR "Compiler warnings must not modify CMAKE_CXX_FLAGS")
endif()

foreach(expected_warning_scope IN ITEMS target_compile_options MSVC AppleClang)
    string(FIND "${project_cmake}" "${expected_warning_scope}" warning_scope_index)
    if(warning_scope_index EQUAL -1)
        message(FATAL_ERROR "Missing target warning configuration: ${expected_warning_scope}")
    endif()
endforeach()