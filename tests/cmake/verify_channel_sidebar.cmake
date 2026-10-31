if(NOT DEFINED BRANCHTALK_DESKTOP_SOURCE_DIR)
    message(FATAL_ERROR "BRANCHTALK_DESKTOP_SOURCE_DIR is required")
endif()

set(model_header "${BRANCHTALK_DESKTOP_SOURCE_DIR}/channel_list_model.hpp")
set(model_source "${BRANCHTALK_DESKTOP_SOURCE_DIR}/channel_list_model.cpp")
set(main_qml "${BRANCHTALK_DESKTOP_SOURCE_DIR}/Main.qml")

foreach(required_file IN ITEMS "${model_header}" "${model_source}" "${main_qml}")
    if(NOT EXISTS "${required_file}")
        message(FATAL_ERROR "Required channel sidebar file was not found: ${required_file}")
    endif()
endforeach()

file(READ "${model_header}" header_contents)
file(READ "${model_source}" source_contents)
file(READ "${main_qml}" main_contents)

foreach(required_text IN ITEMS
        "class ChannelListModel"
        "Q_PROPERTY(QString searchQuery"
        "Q_PROPERTY(int matchingChannelCount"
        "Q_PROPERTY(QString selectedChannelId"
        "Q_PROPERTY(QString selectedChannelName"
        "TextChannel"
        "VoiceChannel"
        "KindRole"
        "UnreadCountRole"
        "MutedRole"
        "MatchesSearchRole"
        "Q_INVOKABLE void setSearchQuery"
        "Q_INVOKABLE bool selectChannel")
    string(FIND "${header_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR
            "ChannelListModel is missing state or role contract: ${required_text}")
    endif()
endforeach()

foreach(required_text IN ITEMS
        "{KindRole, \"kind\"}"
        "{UnreadCountRole, \"unreadCount\"}"
        "{MutedRole, \"muted\"}"
        "{MatchesSearchRole, \"matchesSearch\"}"
        "dataChanged(index(0, 0), index(rowCount() - 1, 0), {MatchesSearchRole})"
        "dataChanged(changed_index, changed_index, {UnreadCountRole})"
        "dataChanged(changed_index, changed_index, {MutedRole})")
    string(FIND "${source_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR
            "ChannelListModel is missing targeted model updates: ${required_text}")
    endif()
endforeach()

foreach(forbidden_text IN ITEMS "beginResetModel" "endResetModel")
    string(FIND "${source_contents}" "${forbidden_text}" forbidden_index)
    if(NOT forbidden_index EQUAL -1)
        message(FATAL_ERROR
            "ChannelListModel must update roles without resetting: ${forbidden_text}")
    endif()
endforeach()

foreach(required_text IN ITEMS
        "required property ChannelListModel channelModel"
        "property bool channelsCollapsed"
        "objectName: \"channelSearch\""
        "window.channelModel.setSearchQuery(text)"
        "model: window.channelModel"
        "required property int kind"
        "required property int unreadCount"
        "required property bool muted"
        "required property bool matchesSearch"
        "active: selected"
        "window.channelModel.selectChannel(stableId)"
        "window.channelModel.selectedChannelName")
    string(FIND "${main_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR
            "Main.qml is missing a channel model binding: ${required_text}")
    endif()
endforeach()

string(FIND "${main_contents}" "readonly property var channels" local_channel_model)
if(NOT local_channel_model EQUAL -1)
    message(FATAL_ERROR "Main.qml must use the injected channel model")
endif()