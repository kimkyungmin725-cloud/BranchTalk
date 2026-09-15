cmake_minimum_required(VERSION 3.25)

project(BranchTalkForge VERSION 0.1.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(BranchTalkTargets)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)


add_library(branchtalk_core STATIC
    libs/core/src/application_config.cpp
    libs/core/src/dependency_versions.cpp
    libs/core/src/logging.cpp
    libs/core/src/version.cpp
)

add_library(BranchTalk::core ALIAS branchtalk_core)

target_include_directories(branchtalk_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/core/include"
)

target_link_libraries(branchtalk_core
    PRIVATE
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)

branchtalk_configure_cpp_target(branchtalk_core PUBLIC)

add_executable(branchtalk_client
    apps/client/main.cpp
)

target_link_libraries(branchtalk_client
    PRIVATE
        BranchTalk::core
)

branchtalk_configure_cpp_target(branchtalk_client PRIVATE)

add_library(branchtalk_server_app STATIC
    apps/server/src/server_app.cpp
)

add_library(BranchTalk::server_app ALIAS branchtalk_server_app)

target_include_directories(branchtalk_server_app
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/apps/server/include"
)

target_link_libraries(branchtalk_server_app
    PUBLIC
        BranchTalk::core
)

branchtalk_configure_cpp_target(branchtalk_server_app PUBLIC)

add_executable(branchtalk_server
    apps/server/main.cpp
)

target_link_libraries(branchtalk_server
    PRIVATE
        BranchTalk::server_app
)

branchtalk_configure_cpp_target(branchtalk_server PRIVATE)

include(CTest)

option(BRANCHTALK_BUILD_DESKTOP "Build the Qt Quick desktop application" OFF)

if(BUILD_TESTING)
    add_subdirectory(tests)
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