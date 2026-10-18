if(NOT DEFINED BRANCHTALK_DESKTOP_SOURCE_DIR)
    message(FATAL_ERROR "BRANCHTALK_DESKTOP_SOURCE_DIR is required")
endif()

set(view_model_header "${BRANCHTALK_DESKTOP_SOURCE_DIR}/app_view_model.hpp")
set(main_qml "${BRANCHTALK_DESKTOP_SOURCE_DIR}/Main.qml")

foreach(required_file IN ITEMS "${view_model_header}" "${main_qml}")
    if(NOT EXISTS "${required_file}")
        message(FATAL_ERROR "Required view-model boundary file was not found: ${required_file}")
    endif()
endforeach()

file(READ "${view_model_header}" view_model_contents)
file(READ "${main_qml}" main_contents)

foreach(required_text IN ITEMS
        "class AppViewModel"
        "Q_PROPERTY(Screen currentScreen"
        "LoginScreen"
        "MainScreen"
        "SettingsScreen"
        "Q_INVOKABLE void showLogin"
        "Q_INVOKABLE void showMain"
        "Q_INVOKABLE void showSettings")
    string(FIND "${view_model_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR "AppViewModel is missing state or command contract: ${required_text}")
    endif()
endforeach()

foreach(required_text IN ITEMS
        "required property AppViewModel appViewModel"
        "appViewModel.currentScreen"
        "AppViewModel.LoginScreen"
        "AppViewModel.MainScreen"
        "AppViewModel.SettingsScreen"
        "onClicked: window.appViewModel.showLogin()"
        "onClicked: window.appViewModel.showMain()"
        "onClicked: window.appViewModel.showSettings()")
    string(FIND "${main_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR "Main.qml is missing a view-model binding: ${required_text}")
    endif()
endforeach()

string(REGEX MATCH
    "property[^\r\n]*currentScreen[ \t]*:"
    local_screen_state
    "${main_contents}")
if(local_screen_state)
    message(FATAL_ERROR "Main.qml must not own the current screen state")
endif()

file(GLOB desktop_qml_files "${BRANCHTALK_DESKTOP_SOURCE_DIR}/*.qml")
foreach(qml_file IN LISTS desktop_qml_files)
    file(READ "${qml_file}" qml_contents)
    foreach(forbidden_text IN ITEMS
            "XMLHttpRequest"
            "WebSocket"
            "QtWebSockets"
            "LocalStorage"
            "openDatabaseSync"
            "executeSql"
            "fetch("
            "QNetworkAccessManager")
        string(FIND "${qml_contents}" "${forbidden_text}" forbidden_index)
        if(NOT forbidden_index EQUAL -1)
            message(FATAL_ERROR
                "QML must not contain network or database logic: ${qml_file}: ${forbidden_text}")
        endif()
    endforeach()
endforeach()
