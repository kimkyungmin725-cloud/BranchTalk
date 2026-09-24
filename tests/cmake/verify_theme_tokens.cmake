if(NOT DEFINED BRANCHTALK_DESKTOP_SOURCE_DIR)
    message(FATAL_ERROR "BRANCHTALK_DESKTOP_SOURCE_DIR is required")
endif()

set(theme_file "${BRANCHTALK_DESKTOP_SOURCE_DIR}/Theme.qml")
if(NOT EXISTS "${theme_file}")
    message(FATAL_ERROR "Theme.qml was not found")
endif()

file(READ "${theme_file}" theme_contents)

foreach(required_text IN ITEMS
        "pragma Singleton"
        "property bool darkMode"
        "windowBackground"
        "panelBackground"
        "textPrimary"
        "textSecondary"
        "accentHovered"
        "accentPressed"
        "spacingSmall"
        "spacingMedium"
        "spacingLarge"
        "titleFontSize"
        "bodyFontSize"
        "buttonFontSize")
    string(FIND "${theme_contents}" "${required_text}" required_index)
    if(required_index EQUAL -1)
        message(FATAL_ERROR "Theme.qml is missing token contract: ${required_text}")
    endif()
endforeach()

foreach(qml_file IN ITEMS Main.qml AppButton.qml AppPanel.qml)
    set(qml_path "${BRANCHTALK_DESKTOP_SOURCE_DIR}/${qml_file}")
    if(NOT EXISTS "${qml_path}")
        message(FATAL_ERROR "${qml_file} was not found")
    endif()

    file(READ "${qml_path}" qml_contents)
    string(FIND "${qml_contents}" "Theme." theme_reference)
    if(theme_reference EQUAL -1)
        message(FATAL_ERROR "${qml_file} must consume Theme tokens")
    endif()

    string(REGEX MATCH "\"#[0-9A-Fa-f]+\"" hardcoded_color "${qml_contents}")
    if(hardcoded_color)
        message(FATAL_ERROR
            "${qml_file} contains a color literal outside Theme.qml: ${hardcoded_color}")
    endif()

    string(REGEX MATCH
        "(spacing|margins|[Pp]adding|radius)[ \\t]*:[ \\t]*[0-9]+"
        hardcoded_spacing
        "${qml_contents}")
    if(hardcoded_spacing)
        message(FATAL_ERROR
            "${qml_file} contains hardcoded spacing outside Theme.qml: ${hardcoded_spacing}")
    endif()
endforeach()

foreach(required_component IN ITEMS AppButton AppPanel)
    file(READ "${BRANCHTALK_DESKTOP_SOURCE_DIR}/Main.qml" main_contents)
    string(FIND "${main_contents}" "${required_component} {" component_index)
    if(component_index EQUAL -1)
        message(FATAL_ERROR "Main.qml must use ${required_component}")
    endif()
endforeach()