# CMakeLists.txt for MediaSonic
#
# TM & (C) 2025 Syndromatic Ltd. All rights reserved.
# Designed by Kavish Krishnakumar in Manchester.
#
# This project is licensed under the GNU General Public License v2.0.

cmake_minimum_required(VERSION 3.5)

project(MediaSonic VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find necessary Qt5 packages
find_package(Qt5 COMPONENTS Core Gui Widgets Multimedia MultimediaWidgets Concurrent Xml REQUIRED)
list(APPEND CMAKE_MODULE_PATH "/usr/share/ECM/modules")
list(APPEND CMAKE_MODULE_PATH "/usr/share/ECM/find-modules")
find_package(KF5 COMPONENTS I18n CoreAddons WidgetsAddons REQUIRED)

# Automatically run moc, uic, and rcc for build automation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Define the source files for the project
set(PROJECT_SOURCES
    src/main.cpp
    src/application.cpp
    src/application.h
    src/mainwindow.cpp
    src/mainwindow.h
    src/mediaplayer.cpp
    src/mediaplayer.h
    # Custom Widgets
    src/lcddisplay.cpp
    src/lcddisplay.h
    src/topbar.cpp
    src/topbar.h
    src/sidebar.cpp
    src/sidebar.h
    # Dialogs
    src/dialogs/aboutInfo.cpp
    src/dialogs/aboutInfo.h
    # Cover Flow (from DocSurf)
    src/flow.cpp
    src/flow.h
    # Resources
    src/resources.qrc
)

# Add an executable for MediaSonic
add_executable(MediaSonic ${PROJECT_SOURCES})

# Add the src directory to the include path
target_include_directories(MediaSonic PRIVATE src)

# Link the necessary libraries
target_link_libraries(MediaSonic
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Multimedia
    Qt5::MultimediaWidgets
    Qt5::Concurrent
    Qt5::Xml
    KF5::I18n
    KF5::CoreAddons
    KF5::WidgetsAddons
)

# Installation path
install(TARGETS MediaSonic
    RUNTIME DESTINATION bin
)

# Add uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
