add_test(NAME branchtalk_client.smoke COMMAND branchtalk_client)

set_tests_properties(branchtalk_client.smoke PROPERTIES
    PASS_REGULAR_EXPRESSION "\\[client\\] application started"
)

add_test(
    NAME branchtalk_server.smoke
    COMMAND
        "${CMAKE_COMMAND}"
        "-DBRANCHTALK_SERVER=$<TARGET_FILE:branchtalk_server>"
        "-DBRANCHTALK_INVALID_CONFIG=${CMAKE_CURRENT_SOURCE_DIR}/fixtures/config/invalid-log-level.json"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/verify_server.cmake"
)

branchtalk_add_test_executable(branchtalk_server_app_smoke
    SOURCES
        server/server_app_smoke.cpp
    LIBRARIES
        BranchTalk::server_app
)

add_test(
    NAME branchtalk_server.app
    COMMAND branchtalk_server_app_smoke
)

if(BRANCHTALK_ENABLE_CONTRACT_TESTS)
    add_test(
        NAME branchtalk_cmake_presets.contract
        COMMAND
            "${CMAKE_COMMAND}"
            "-DBRANCHTALK_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/verify_presets.cmake"
    )

    add_test(
        NAME branchtalk_architecture.boundaries
        COMMAND
            "${CMAKE_COMMAND}"
            "-DBRANCHTALK_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
            "-DBRANCHTALK_CLIENT_USES_CORE=$<IN_LIST:BranchTalk::core,$<TARGET_PROPERTY:branchtalk_client,LINK_LIBRARIES>>"
            "-DBRANCHTALK_CLIENT_USES_SERVER_APP=$<IN_LIST:BranchTalk::server_app,$<TARGET_PROPERTY:branchtalk_client,LINK_LIBRARIES>>"
            "-DBRANCHTALK_SERVER_USES_CORE=$<IN_LIST:BranchTalk::core,$<TARGET_PROPERTY:branchtalk_server,LINK_LIBRARIES>>"
            "-DBRANCHTALK_SERVER_USES_SERVER_APP=$<IN_LIST:BranchTalk::server_app,$<TARGET_PROPERTY:branchtalk_server,LINK_LIBRARIES>>"
            "-DBRANCHTALK_SERVER_APP_EXPOSES_CORE=$<IN_LIST:BranchTalk::core,$<TARGET_PROPERTY:branchtalk_server_app,INTERFACE_LINK_LIBRARIES>>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/verify_boundaries.cmake"
    )

    set_tests_properties(
        branchtalk_cmake_presets.contract
        branchtalk_architecture.boundaries
        PROPERTIES LABELS "contract"
    )
endif()

branchtalk_add_test_executable(branchtalk_core_dependencies_smoke
    SOURCES
        core/dependency_versions_smoke.cpp
    LIBRARIES
        BranchTalk::core
)

add_test(
    NAME branchtalk_core.dependencies
    COMMAND branchtalk_core_dependencies_smoke
)

branchtalk_add_test_executable(branchtalk_core_result_smoke
    SOURCES
        core/result_smoke.cpp
    LIBRARIES
        BranchTalk::core
)

add_test(
    NAME branchtalk_core.result
    COMMAND branchtalk_core_result_smoke
)

branchtalk_add_test_executable(branchtalk_core_application_config_smoke
    SOURCES
        core/application_config_smoke.cpp
    LIBRARIES
        BranchTalk::core
)

add_test(
    NAME branchtalk_core.application_config
    COMMAND
        branchtalk_core_application_config_smoke
        "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/config"
)

branchtalk_add_test_executable(branchtalk_core_logging_smoke
    SOURCES
        core/logging_smoke.cpp
    LIBRARIES
        BranchTalk::core
)

add_test(
    NAME branchtalk_core.logging
    COMMAND
        "${CMAKE_COMMAND}"
        "-DBRANCHTALK_LOGGING_SMOKE=$<TARGET_FILE:branchtalk_core_logging_smoke>"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/verify_logging.cmake"
)

branchtalk_add_test_executable(branchtalk_core_public_api_smoke
    SOURCES
        core/public_api_smoke.cpp
    LIBRARIES
        BranchTalk::core
)

add_test(
    NAME branchtalk_core.public_api
    COMMAND branchtalk_core_public_api_smoke
)

set_tests_properties(
    branchtalk_client.smoke
    branchtalk_server.app
    branchtalk_core.dependencies
    branchtalk_core.result
    branchtalk_core.application_config
    branchtalk_core.public_api
    PROPERTIES LABELS "fast"
)

set_tests_properties(
    branchtalk_server.smoke
    branchtalk_core.logging
    PROPERTIES LABELS "integration"
)
