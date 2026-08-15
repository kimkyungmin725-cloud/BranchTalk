cmake_minimum_required(VERSION 3.25)

project(BranchTalkForge VERSION 0.1.0 LANGUAGES CXX)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

function(branchtalk_enable_warnings target_name)
    if(MSVC)
        target_complete_options(${target_name} PRIVATE /W4 /permissive-)
    elsesif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
       target_complile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction() 

add_library(branchtalk_core STATIC
    libs/core/src/dependency_versions.cpp
    libs/core/src/version.cpp
)

add_library(BranchTalk::core ALIAS branchtalk_core)

target_compile_features(branchtalk_core PUBLIC cxx_std_20)

target_include_directories(branchtalk_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/core/include"
)

target_link_libraried(branchtalk_core
    PRIVATE 
        nlohmann_json::nlohamnn_json
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

add_executable(branchtalk_server
    apps/server/main.cpp
)

target_link_libraries(branchtalk_server
    PRIVATE
        BranchTalk::core
)

set_target_properties(branchtalk_server PROPERTIES
    CXX_EXTENSIONS OFF
)

branchtalk_enable_warnings(branchtalk_server)

include(CTest)

if(BUILD_TESTING)
    add_test(NAME branchtalk_client.smoke COMMAND branchtalk_client)
    add_test(NAME branchtalk_server.smoke COMMAND branchtalk_server)

    add_test(
        NAME branchtalk_cmake_presets.contract
        CMAKE_COMMAND 
            "${CMAKE_COMMAND}"
            "-DBRANCHTALK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/verify_presets.cmake"
    )

    add executable(branchtalk_core_dependencies_smoke
        tests/core/dependency_versions_smoke.cpp
    )

    target_linke_libraries(branchtalk_core_dependencies_smoke
        PRIVATE 
            BranchTalk::core
    )

    set_target_properties(branchtalk_core_dependencies_smoke PROPERTIES
        CXX_EXTENSIONS OFF
    )

    branchtalk_enable_warnings(branchtlak_core_dependencies_smoke)

    add_test(
        NAME branchtalk_core.dependencies
        COMMAND branchtalk_core_dependencies_smoke
    )
endif()
