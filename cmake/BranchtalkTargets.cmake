include_guard(GLOBAL)

function(branchtalk_configure_cpp_target target_name standard_scope)
    if(NOT TARGET "${target_name}")
        message(FATAL_ERROR "Cannot configure missing target: ${target_name}")
    endif()

    if(NOT standard_scope STREQUAL "PRIVATE" AND
       NOT standard_scope STREQUAL "PUBLIC")
        message(FATAL_ERROR
            "C++ standard scope for ${target_name} must be PRIVATE or PUBLIC")
    endif()

    target_compile_features(${target_name} ${standard_scope} cxx_std_20)

    set_target_properties(${target_name} PROPERTIES
        CXX_EXTENSIONS OFF
    )

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive-)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

function(branchtalk_add_test_executable target_name)
    cmake_parse_arguments(PARSE_ARGV 1 test "" "" "SOURCES;LIBRARIES")

    if(test_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Unexpected arguments for ${target_name}: ${test_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT test_SOURCES)
        message(FATAL_ERROR "Test target ${target_name} requires SOURCES")
    endif()

    add_executable(${target_name} ${test_SOURCES})

    if(test_LIBRARIES)
        target_link_libraries(${target_name} PRIVATE ${test_LIBRARIES})
    endif()

    branchtalk_configure_cpp_target(${target_name} PRIVATE)
endfunction()