cmake_minimum_required(VERSION 3.19)
project(UsefulClasses LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Xml Charts)

qt_standard_project_setup()

qt_add_executable(UsefulClasses
    WIN32 MACOSX_BUNDLE
    main.cpp
    testwindow.cpp
    testwindow.h
    testwindow.ui

    mtsettings.h mtsettings.cpp
    mtqss.h mtqss.cpp
    chartcomment.h chartcomment.cpp
    testchart.h testchart.cpp
)

qt_add_translations(
    TARGETS UsefulClasses
    TS_FILES UsefulClasses_pl_PL.ts
)

target_link_libraries(UsefulClasses
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Xml
        Qt::Charts
)

include(GNUInstallDirs)

install(TARGETS UsefulClasses
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET UsefulClasses
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
