foreach(required_variable IN ITEMS
        BRANCHTALK_CORE_TARGET
        BRANCHTALK_SERVER_TARGET
        BRANCHTALK_DESKTOP_TARGET)
    if(NOT DEFINED ${required_variable})
        message(FATAL_ERROR "${required_variable} is required")
    endif()
endforeach()

if(NOT BRANCHTALK_CORE_TARGET)
    message(FATAL_ERROR "branchtalk_core must remain available when the desktop target is disabled")
endif()

if(NOT BRANCHTALK_SERVER_TARGET)
    message(FATAL_ERROR "branchtalk_server must remain available when the desktop target is disabled")
endif()

if(BRANCHTALK_DESKTOP_TARGET)
    message(FATAL_ERROR "branchtalk_desktop must not exist when desktop support is disabled")
endif()