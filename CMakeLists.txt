cmake_minimum_required(VERSION 3.25)

project(BranchTalkForge VERSION 0.1.0 LANGUAGES CXX)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

function(branchtalk_enable_warnings target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive-)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

add_library(branchtalk_core STATIC
    libs/core/src/application_config.cpp
    libs/core/src/dependency_versions.cpp
    libs/core/src/logging.cpp
    libs/core/src/version.cpp
)

add_library(BranchTalk::core ALIAS branchtalk_core)

target_compile_features(branchtalk_core PUBLIC cxx_std_20)

target_include_directories(branchtalk_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/core/include"
)

target_link_libraries(branchtalk_core
    PRIVATE
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)

set_target_properties(branchtalk_core PROPERTIES
    CXX_EXTENSIONS OFF
)

branchtalk_enable_warnings(branchtalk_core)

add_executable(branchtalk_client
    apps/client/main.cpp
)

target_link_libraries(branchtalk_client
    PRIVATE
        BranchTalk::core
)

set_target_properties(branchtalk_client PROPERTIES
    CXX_EXTENSIONS OFF
)

branchtalk_enable_warnings(branchtalk_client)

add_library(branchtalk_server_app STATIC
    apps/server/src/server_app.cpp
)

add_library(BranchTalk::server_app ALIAS branchtalk_server-app)

target_compile_features(branchtalk_server_app PUBLIC cxx_std_20)

target_include_directories(branchtalk_server_app
    PUBLIC 
        "${CMAKE_CURRENT_SOURCE_DIR}/apps/server/include"
)

target_linke_libraries(branchtalk_server.app
    PUBLIC 
        Branchtalk::core
)

set_target_properties(branchtalk_server_app PROPERTIES 
    CXX_EXTENSIONS OFF
)

branchtalk_enable_warnings(branchtalk_server_app)

add_executable(branchtalk_server
    apps/server/main.cpp
)

target_link_libraries(branchtalk_server
    PRIVATE
        BranchTalk::server_app
)

set_target_properties(branchtalk_server PROPERTIES
    CXX_EXTENSIONS OFF
)

branchtalk_enable_warnings(branchtalk_server)

include(CTest)

option(BRANCHTALK_BUILD_DESKTOP "Build the Qt Quick desktop application" OFF)

if(BUILD_TESTING)
    add_test(NAME branchtalk_client.smoke COMMAND branchtalk_client)
    
    add_test(
        NAME branchtalk_server.smoke
        COMMAND 
            "${CMAKE_COMMAND}"
            "-DBRANCHTALK_SERVER=$<TARGET_FILE:branchtalk_server>"
            "-DBRANCHTALK_INVALID_CONFIG=${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/config/invalid-log-level.json"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/verify_server.cmake"
    )

    set_tests_properties(branchtalk_client.smoke PROPERTIES
        PASS_REGULAR_EXPRESSION "\\[client\\] application started"
    )

    add_executable(branchtalk_server_app_smoke
        tests/server/server_app_smoke.cpp
    )

    target_link_libraries(branchtalk_server_app_smoke
        PRIVATE 
            Branchtalk::server_app
    )

    set_target_properties(branchtalk_server_app_smoke PROPERTIES 
        CXX_EXTENSIONS OFF
    )

    branchtalk_enable_warnings(branchtalk_server_app_smoke)

    add_test(
        NAME branchtalk_server.app
        COMMAND branchtalk_server_app_smoke
    )

    add_test(
        NAME branchtalk_cmake_presets.contract
        COMMAND
            "${CMAKE_COMMAND}"
            "-DBRANCHTALK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/verify_presets.cmake"
    )

    add_executable(branchtalk_core_dependencies_smoke
        tests/core/dependency_versions_smoke.cpp
    )

    target_link_libraries(branchtalk_core_dependencies_smoke
        PRIVATE
            BranchTalk::core
    )

    set_target_properties(branchtalk_core_dependencies_smoke PROPERTIES
        CXX_EXTENSIONS OFF
    )

    branchtalk_enable_warnings(branchtalk_core_dependencies_smoke)

    add_test(
        NAME branchtalk_core.dependencies
        COMMAND branchtalk_core_dependencies_smoke
    )

    add_executable(branchtalk_core_result_smoke
        tests/core/result_smoke.cpp
    )

    target_link_libraries(branchtalk_core_result_smoke
        PRIVATE
            BranchTalk::core
    )

    set_target_properties(branchtalk_core_result_smoke PROPERTIES
        CXX_EXTENSIONS OFF
    )

    branchtalk_enable_warnings(branchtalk_core_result_smoke)

    add_test(
        NAME branchtalk_core.result
        COMMAND branchtalk_core_result_smoke
    )

    add_executable(branchtalk_core_application_config_smoke
        tests/core/application_config_smoke.cpp
    )

    target_link_libraries(branchtalk_core_application_config_smoke
        PRIVATE
            BranchTalk::core
    )

    set_target_properties(branchtalk_core_application_config_smoke PROPERTIES
        CXX_EXTENSIONS OFF
    )

    branchtalk_enable_warnings(branchtalk_core_application_config_smoke)

    add_test(
        NAME branchtalk_core.application_config
        COMMAND
            branchtalk_core_application_config_smoke
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/config"
    )

    add_executable(branchtalk_core_logging_smoke
        tests/core/logging_smoke.cpp
    )

    target_link_libraries(branchtalk_core_logging_smoke
        PRIVATE
            BranchTalk::core
    )

    set_target_properties(branchtalk_core_logging_smoke PROPERTIES
        CXX_EXTENSIONS OFF
    )

    branchtalk_enable_warnings(branchtalk_core_logging_smoke)

    add_test(
        NAME branchtalk_core.logging
        COMMAND
            "${CMAKE_COMMAND}"
            "-DBRANCHTALK_LOGGING_SMOKE=$<TARGET_FILE:branchtalk_core_logging_smoke>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/verify_logging.cmake"
    )
endif()

if(BRANCHTALK_BUILD_DESKTOP)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)
    qt_standard_project_setup(REQUIRES 6.5)
    add_subdirectory(apps/desktop)
elseif(BUILD_TESTING)
    add_test(
        NAME branchtalk_desktop.disabled
        COMMAND
            "${CMAKE_COMMAND}"
            "-DBRANCHTALK_CORE_TARGET=$<TARGET_EXISTS:branchtalk_core>"
            "-DBRANCHTALK_SERVER_TARGET=$<TARGET_EXISTS:branchtalk_server>"
            "-DBRANCHTALK_DESKTOP_TARGET=$<TARGET_EXISTS:branchtalk_desktop>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/verify_desktop_disabled.cmake"
    )
endif()