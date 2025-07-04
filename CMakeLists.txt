# CMakeLists.txt for the SynOS Music Player
#
# TM & (C) 2025 Syndromatic Ltd. All rights reserved.
# Designed by Kavish Krishnakumar in Manchester.
#
# This project is licensed under the GNU General Public License v2.0.
# See the LICENSE file for more details.

cmake_minimum_required(VERSION 3.5)

project(SynOSMusicPlayer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatically run moc, uic, and rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find necessary Qt5 packages
find_package(Qt5 COMPONENTS Core Gui Widgets Multimedia Concurrent REQUIRED)
find_package(KF5 COMPONENTS I18n CoreAddons WidgetsAddons REQUIRED)

# Define the source files for the project
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    flow.cpp
    flow.h
    mediaplayer.cpp
    mediaplayer.h
    aboutdialog.cpp
    aboutdialog.h
    application.cpp
    application.h
)

# Add an executable for the music player
add_executable(SynOSMusicPlayer ${PROJECT_SOURCES})

# Link the necessary libraries
target_link_libraries(SynOSMusicPlayer
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Multimedia
    Qt5::Concurrent
    KF5::I18n
    KF5::CoreAddons
    KF5::WidgetsAddons
)

# Installation
install(TARGETS SynOSMusicPlayer
    RUNTIME DESTINATION bin
)
