cmake_minimum_required(VERSION 3.16)

project(untitled VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_GENERATE_QMLLS_INI ON)

# ✅ Добавить Qml
find_package(Qt6 6.8 REQUIRED COMPONENTS Quick Qml)

qt_standard_project_setup(REQUIRES 6.8)

# ✅ Упрощённый вариант — просто указать путь
set(QML_IMPORT_PATH "C:/msys64/ucrt64/qml" CACHE STRING "QML import paths" FORCE)

qt_add_executable(appuntitled
    main.cpp
)

qt_add_qml_module(appuntitled
    URI untitled
    VERSION 1.0
    QML_FILES
        Main.qml
)

set_target_properties(appuntitled PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appuntitled
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appuntitled
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)