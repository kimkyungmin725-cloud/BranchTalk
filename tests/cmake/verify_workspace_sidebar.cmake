if(NOT DEFINED BRANCHTALK_DESKTOP_SOURCE_DIR)
    message(FATAL_ERROR "BRANCHTALK_DESKTOP_SOURCE_DIR is required")
endif()

set(model_header "${BRANCHTALK_DESKTOP_SOURCE_DIR}/workspace_list_model.hpp")
set(model_source "${BRANCHTALK_DESKTOP_SOURCE_DIR}/workspace_list_model.cpp")
set(main_qml "${BRANCHTALK_DESKTOP_SOURCE_DIR}/Main.qml")

foreach(required_file IN ITEMS "${model_header}" "${model_source}" "${main_qml}")
    if(NOT EXISTS "${required_file}")
        message(FATAL_ERROR "Required workspace sidebar file was not found: ${required_file}")
    endif()
endforeach()

file(READ "${model_header}" header_contents)
file(READ "${model_source}" source_contents)
file(READ "${main_qml}" main_contents)

foreach(required_text IN ITEMS
        "class WorkspaceListModel"
        "Q_PROPERTY(bool loading"
        "Q_PROPERTY(bool empty"
        "Q_PROPERTY(QString selectedWorkspaceId"
        "StableIdRole"
        "NameRole"
        "SelectedRole"
        "UnreadCountRole"
        "Q_INVOKABLE bool selectWorkspace")
    string(FIND "${header_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR
            "WorkspaceListModel is missing state or role contract: ${required_text}")
    endif()
endforeach()

foreach(required_text IN ITEMS
        "{StableIdRole, \"stableId\"}"
        "{NameRole, \"name\"}"
        "{SelectedRole, \"selected\"}"
        "{UnreadCountRole, \"unreadCount\"}"
        "indexOfStableId(selected_workspace_id_)"
        "beginInsertRows"
        "beginRemoveRows")
    string(FIND "${source_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR
            "WorkspaceListModel is missing model behavior: ${required_text}")
    endif()
endforeach()

foreach(required_text IN ITEMS
        "required property WorkspaceListModel workspaceModel"
        "model: window.workspaceModel"
        "required property string stableId"
        "required property bool selected"
        "required property int unreadCount"
        "active: selected"
        "window.workspaceModel.selectWorkspace(stableId)"
        "visible: window.workspaceModel.loading"
        "visible: !window.workspaceModel.loading && window.workspaceModel.empty")
    string(FIND "${main_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR
            "Main.qml is missing a workspace model binding: ${required_text}")
    endif()
endforeach()

string(FIND "${main_contents}" "readonly property var workspaces" local_workspace_model)
if(NOT local_workspace_model EQUAL -1)
    message(FATAL_ERROR "Main.qml must use the injected workspace model")
endif()
