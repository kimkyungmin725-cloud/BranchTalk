if(NOT DEFINED BRANCHTALK_DESKTOP_SOURCE_DIR)
    message(FATAL_ERROR "BRANCHTALK_DESKTOP_SOURCE_DIR is required")
endif()

set(state_header "${BRANCHTALK_DESKTOP_SOURCE_DIR}/message_composer_state.hpp")
set(state_source "${BRANCHTALK_DESKTOP_SOURCE_DIR}/message_composer_state.cpp")
set(desktop_main "${BRANCHTALK_DESKTOP_SOURCE_DIR}/main.cpp")
set(composer_qml "${BRANCHTALK_DESKTOP_SOURCE_DIR}/MessageComposer.qml")
set(main_qml "${BRANCHTALK_DESKTOP_SOURCE_DIR}/Main.qml")
set(readme "${BRANCHTALK_DESKTOP_SOURCE_DIR}/../../README.md")

foreach(required_file IN ITEMS
        "${state_header}"
        "${state_source}"
        "${desktop_main}"
        "${composer_qml}"
        "${main_qml}"
        "${readme}")
    if(NOT EXISTS "${required_file}")
        message(FATAL_ERROR "Required message composer file was not found: ${required_file}")
    endif()
endforeach()

file(READ "${state_header}" header_contents)
file(READ "${state_source}" source_contents)
file(READ "${desktop_main}" main_cpp_contents)
file(READ "${composer_qml}" composer_contents)
file(READ "${main_qml}" main_contents)
file(READ "${readme}" readme_contents)

foreach(required_text IN ITEMS
        "class MessageComposerState"
        "Q_PROPERTY(QString text"
        "Q_PROPERTY(int characterCount"
        "Q_PROPERTY(bool canSend"
        "bool submit(bool input_method_composing)"
        "void messageSubmitted")
    string(FIND "${header_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR "MessageComposerState is missing its contract: ${required_text}")
    endif()
endforeach()

foreach(required_text IN ITEMS
        "!text_.trimmed().isEmpty()"
        "input_method_composing || !canSend()"
        "emit messageSubmitted(submitted_text)")
    string(FIND "${source_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR "MessageComposerState is missing guarded submission: ${required_text}")
    endif()
endforeach()

foreach(required_text IN ITEMS
        "required property MessageComposerState composerState"
        "inputMethodComposing: messageInput.inputMethodComposing"
        "canSend: composerState.canSend && !inputMethodComposing"
        "Qt.Key_Return"
        "Qt.Key_Enter"
        "Qt.ShiftModifier"
        "composer.composerState.submit(inputMethodComposing)"
        "objectName: \"messageComposerStatus\""
        "objectName: \"messageComposerCharacterCount\""
        "enabled: composer.canSend")
    string(FIND "${composer_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR "MessageComposer.qml is missing input behavior: ${required_text}")
    endif()
endforeach()

foreach(required_text IN ITEMS
        "required property MessageComposerState messageComposerState"
        "MessageComposer {"
        "composerState: window.messageComposerState"
        "anchors.bottom: messageComposer.top")
    string(FIND "${main_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR "Main.qml is missing the composer binding: ${required_text}")
    endif()
endforeach()

string(FIND "${main_contents}" "objectName: \"messageList\"" message_list_index)
string(FIND "${main_contents}" "anchors.bottom: messageComposer.top" message_list_bottom_index)
string(FIND "${main_contents}" "MessageComposer {" message_composer_index)
if(message_list_index EQUAL -1 OR message_list_bottom_index EQUAL -1 OR
   message_composer_index EQUAL -1 OR
   message_list_bottom_index LESS message_list_index OR
   message_list_bottom_index GREATER message_composer_index)
    message(FATAL_ERROR "The message list must end above the composer")
endif()

foreach(required_text IN ITEMS
        "UiRegressionScenario::MessageComposer"
        "message_composer_handles_input")
    string(FIND "${main_cpp_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR "The UI flow is missing composer coverage: ${required_text}")
    endif()
endforeach()

foreach(required_text IN ITEMS
        "Windows"
        "macOS"
        "Shift+Enter"
        "IME composition")
    string(FIND "${readme_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR "README is missing composer shortcut documentation: ${required_text}")
    endif()
endforeach()

string(FIND "${composer_contents}" "DropArea" drop_area_index)
if(NOT drop_area_index EQUAL -1)
    message(FATAL_ERROR "MessageComposer.qml must not include attachment drop handling")
endif()
