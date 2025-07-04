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
find_package(Qt5 COMPONENTS Core Gui Widgets Multimedia Concurrent Xml REQUIRED)
find_package(KF5 COMPONENTS I18n CoreAddons WidgetsAddons REQUIRED)

# Automatically run moc, uic, and rcc for build automation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Define the source files for the project
set(PROJECT_SOURCES
    main.cpp
    application.cpp
    application.h
    mainwindow.cpp
    mainwindow.h
    mediaplayer.cpp
    mediaplayer.h
    # Custom Widgets
    lcddisplay.cpp
    lcddisplay.h
    topbar.cpp
    topbar.h
    sidebar.cpp
    sidebar.h
    # Dialogs
    dialogs/aboutinfo.cpp
    dialogs/aboutinfo.h
    # Cover Flow (from DocSurf)
    flow.cpp
    flow.h
    # Resources
    resources.qrc
)

# Add an executable for MediaSonic
add_executable(MediaSonic ${PROJECT_SOURCES})

# Link the necessary libraries
target_link_libraries(MediaSonic
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Multimedia
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
