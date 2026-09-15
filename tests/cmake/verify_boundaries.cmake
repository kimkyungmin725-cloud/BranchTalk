foreach(required_variable IN ITEMS
        BRANCHTALK_SOURCE_DIR
        BRANCHTALK_CLIENT_USES_CORE
        BRANCHTALK_CLIENT_USES_SERVER_APP
        BRANCHTALK_SERVER_USES_CORE
        BRANCHTALK_SERVER_USES_SERVER_APP
        BRANCHTALK_SERVER_APP_EXPOSES_CORE)
    if(NOT DEFINED ${required_variable})
        message(FATAL_ERROR "${reqeuired_variable} is required")
    endif()
endforeach()

if(NOT BRANCHTALK_CLIENT_USES_CORE)
    message(FATAL_ERROR "branchtalk_client must link directly to BranchTalk::core")
endif()

if(BRANCHTALK_CLIENT_USES_SERVER_APP)
    message(FATAL_ERROR "branchtalk_client must not link to BranchTalk::server_app")
endif()

if(BRANCHTALK_SERVER_USES_CORE)
    message(FATAL_ERROR
        "branchtalk_server must receive core through BranchTalk::server_app")
endif()

if(NOT BRANCHTALK_SERVER_USES_SERVER_APP)
    message(FATAL_ERROR "branchtalk_server must link to BranchTalk::server_app")
endif()

if(NOT BRANCHTALK_SERVER_APP_EXPOSES_CORE)
    message(FATAL_ERROR "BranchTalk::server_app must expose BranchTalk::core")
endif()

file(GLOB_RECURSE client_sources LIST_DIRECTORIES false
    "${BRANCHTALK_SOURCE_DIR}/apps/client/*.cc"
    "${BRANCHTALK_SOURCE_DIR}/apps/client/*.cpp"
    "${BRANCHTALK_SOURCE_DIR}/apps/client/*.cxx"
    "${BRANCHTALK_SOURCE_DIR}/apps/client/*.h"
    "${BRANCHTALK_SOURCE_DIR}/apps/client/*.hh"
    "${BRANCHTALK_SOURCE_DIR}/apps/client/*.hpp"
    "${BRANCHTALK_SOURCE_DIR}/apps/client/*.hxx"
    "${BRANCHTALK_SOURCE_DIR}/apps/client/*.ixx"
)

if(NOT client_sources)
    message(FATAL_ERROR "No client sources were found")
endif()

foreach(client_source IN LISTS client_sources)
    file(READ "${client_source}" client_sources)
    string(REGEX MATCHALL "#[ \t]*include[^\r\n]*" client_includes "${client_contents}")

    foreach(client_include IN LISTS client_includes)
        foreach(forbidden_path IN ITEMS "branchtalk/server/" "apps/server/" "../server/")
            string(FIND "${client_include}" "${forbidden_path}" forbidden_index)
            if(NOT forbidden_index EQUAL -1)
                message(FATAL_ERROR
                    "Client source includes a server implementation boundary: "
                    "${client_source}: ${client_include}")
            endif()
        endforeach()
    endforeach()
endforeach()