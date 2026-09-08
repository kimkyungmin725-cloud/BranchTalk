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

target_compile_features(branchtalk_desktop PRIVATE cxx_std_20)

target_link_libraries(branchtalk_desktop
    PRIVATE
        BranchTalk::core
        Qt6::Quick
)

set_target_properties(branchtalk_desktop PROPERTIES
    CXX_EXTENSIONS OFF
)

branchtalk_enable_warnings(branchtalk_desktop)

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