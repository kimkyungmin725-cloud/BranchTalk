cmake_minimum_required(VERSION 3.25)

project(BranchTalkForge VERSION 0.1.0 LANGUAGES CXX)

add_library(branchtalk_core STATIC
    libs/core/src/version.cpp
)

add_library(BranchTalk::core ALIAS branchtalk_core)

target_compile_features(branchtalk_core PUBLIC cxx_std_20)

target_include_directories(branchtalk_core
    PUBLIC
        "${CMAKE_CRRENT_SOURCE_DIR}/libs/core/include"
)

set_target_properties(branchtalk_core PROPERTIES
    CXX_EXTENSIONS OFF
)

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

include(CTest)

if(BUILD_TESTING)
    add_test(NAME branchtalk_client.smoke COMMAND branchtalk_client)
    add_test(NAME branchtalk_server.smoke COMMAND branchtalk_server)
endif()