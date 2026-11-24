if(NOT DEFINED BRANCHTALK_DESKTOP_SOURCE_DIR)
    message(FATAL_ERROR "BRANCHTALK_DESKTOP_SOURCE_DIR is required")
endif()

set(model_header "${BRANCHTALK_DESKTOP_SOURCE_DIR}/message_list_model.hpp")
set(model_source "${BRANCHTALK_DESKTOP_SOURCE_DIR}/message_list_model.cpp")
set(desktop_main "${BRANCHTALK_DESKTOP_SOURCE_DIR}/main.cpp")
set(main_qml "${BRANCHTALK_DESKTOP_SOURCE_DIR}/Main.qml")
set(delegate_qml "${BRANCHTALK_DESKTOP_SOURCE_DIR}/MessageDelegate.qml")

foreach(required_file IN ITEMS
        "${model_header}"
        "${model_source}"
        "${desktop_main}"
        "${main_qml}"
        "${delegate_qml}")
    if(NOT EXISTS "${required_file}")
        message(FATAL_ERROR "Required message list file was not found: ${required_file}")
    endif()
endforeach()

file(READ "${model_header}" header_contents)
file(READ "${model_source}" source_contents)
file(READ "${desktop_main}" main_cpp_contents)
file(READ "${main_qml}" main_contents)
file(READ "${delegate_qml}" delegate_contents)

foreach(required_text IN ITEMS
        "class MessageListModel"
        "Pending"
        "Sent"
        "Failed"
        "StableIdRole"
        "AuthorIdRole"
        "AuthorNameRole"
        "TimestampRole"
        "BodyRole"
        "StatusRole"
        "GroupedWithPreviousRole"
        "bool updateMessage")
    string(FIND "${header_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR "MessageListModel is missing its contract: ${required_text}")
    endif()
endforeach()

foreach(required_text IN ITEMS
        "{StableIdRole, \"stableId\"}"
        "{AuthorIdRole, \"authorId\"}"
        "{AuthorNameRole, \"authorName\"}"
        "{TimestampRole, \"timestamp\"}"
        "{BodyRole, \"body\"}"
        "{StatusRole, \"status\"}"
        "{GroupedWithPreviousRole, \"groupedWithPrevious\"}"
        "emit dataChanged(changed_index, changed_index, changed_roles)"
        "emit dataChanged(next_index, next_index, {GroupedWithPreviousRole})")
    string(FIND "${source_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR "MessageListModel is missing targeted updates: ${required_text}")
    endif()
endforeach()

foreach(forbidden_text IN ITEMS "beginResetModel" "endResetModel")
    string(FIND "${source_contents}" "${forbidden_text}" forbidden_index)
    if(NOT forbidden_index EQUAL -1)
        message(FATAL_ERROR "Message updates must not reset the model: ${forbidden_text}")
    endif()
endforeach()

foreach(required_text IN ITEMS
        "required property MessageListModel messageModel"
        "objectName: \"messageList\""
        "reuseItems: true"
        "model: window.messageModel"
        "delegate: MessageDelegate")
    string(FIND "${main_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR "Main.qml is missing a virtualized message binding: ${required_text}")
    endif()
endforeach()

string(FIND "${main_cpp_contents}" "positionViewAtIndex" position_view_index)
if(position_view_index EQUAL -1)
    message(FATAL_ERROR
        "The UI flow must position virtualized rows before reading their delegates")
endif()

string(FIND "${main_contents}" "readonly property var messages" local_messages)
if(NOT local_messages EQUAL -1)
    message(FATAL_ERROR "Main.qml must use the injected message model")
endif()

foreach(required_text IN ITEMS
        "required property string stableId"
        "required property string authorName"
        "required property string timestamp"
        "required property string body"
        "required property int status"
        "required property bool groupedWithPrevious"
        "readonly property bool headerVisible: !groupedWithPrevious"
        "readonly property string statusText: status === MessageListModel.Pending"
        "MessageListModel.Failed")
    string(FIND "${delegate_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR "MessageDelegate.qml is missing display state: ${required_text}")
    endif()
endforeach()
