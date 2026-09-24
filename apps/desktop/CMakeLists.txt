add_library(branchtalk_desktop_window_state STATIC
    window_state_store.cpp
)

target_include_directories(branchtalk_desktop_window_state
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(branchtalk_desktop_window_state
    PUBLIC
        Qt6::Core
)

branchtalk_configure_cpp_target(branchtalk_desktop_window_state PUBLIC)

qt_add_executable(branchtalk_desktop
    WIN32
    MACOSX_BUNDLE
    main.cpp
)

set_source_files_properties(Theme.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(branchtalk_desktop
    URI BranchTalk
    QML_FILES
        AppButton.qml
        AppPanel.qml
        Main.qml
        Theme.qml
)

target_link_libraries(branchtalk_desktop
    PRIVATE
        BranchTalk::core
        branchtalk_desktop_window_state
        Qt6::Quick
        Qt6::QuickControls2
)

branchtalk_configure_cpp_target(branchtalk_desktop PRIVATE)

if(BUILD_TESTING)
    branchtalk_add_test_executable(branchtalk_desktop_window_state_smoke
        SOURCES
            "${PROJECT_SOURCE_DIR}/tests/desktop/window_state_store_smoke.cpp"
        LIBRARIES
            branchtalk_desktop_window_state
    )

    add_test(
        NAME branchtalk_desktop.window_state
        COMMAND branchtalk_desktop_window_state_smoke
    )

    add_test(
        NAME branchtalk_desktop.smoke
        COMMAND branchtalk_desktop --smoke-test
    )

    add_test(
        NAME branchtalk_desktop.theme
        COMMAND branchtalk_desktop --theme-test
    )

    add_test(
        NAME branchtalk_desktop.theme_tokens
        COMMAND
            "${CMAKE_COMMAND}"
            "-DBRANCHTALK_DESKTOP_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
            -P "${PROJECT_SOURCE_DIR}/tests/cmake/verify_theme_tokens.cmake"
    )

    set_tests_properties(
        branchtalk_desktop.smoke
        branchtalk_desktop.theme
        PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_BACKEND=software"
    )

    if(WIN32)
        set_property(
            TEST
                branchtalk_desktop.smoke
                branchtalk_desktop.theme
                branchtalk_desktop.window_state
            APPEND PROPERTY ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
        )
    endif()
endif()