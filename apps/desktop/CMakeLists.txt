qt_add_executable(branchtalk_desktop
    WIN32
    MACOSX_BUNDLE
    main.cpp
)

qt_add_qml_module(branchtalk_desktop
    URI BranchTalk
    QML_FILES
        Main.qml
)

target_link_libraries(branchtalk_desktop
    PRIVATE
        BranchTalk::core
        Qt6::Quick
)

branchtalk_configure_cpp_target(branchtalk_desktop PRIVATE)

if(BUILD_TESTING)
    add_test(
        NAME branchtalk_desktop.smoke
        COMMAND branchtalk_desktop --smoke-test
    )

    set_tests_properties(branchtalk_desktop.smoke PROPERTIES
        ENVIRONMENT
            "QT_QPA_PLATFORM=offscreen;QT_QUICK_BACKEND=software"
    )

    if(WIN32)
        set_property(
            TEST branchtalk_desktop.smoke
            APPEND PROPERTY ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
        )
    endif()
endif()