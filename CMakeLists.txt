# CMakeLists.txt for MediaSonic
#
# TM & (C) 2025 Syndromatic Ltd. All rights reserved.
# Designed by Kavish Krishnakumar in Manchester.
#
# This project is licensed under the GNU General Public License v2.0.

cmake_minimum_required(VERSION 3.16)

project(MediaSonic VERSION 1.0.0 LANGUAGES CXX)

set(MS_CXX_STANDARD 17 CACHE STRING "C++ standard to use (17/20/23)")
set_property(CACHE MS_CXX_STANDARD PROPERTY STRINGS 17 20 23)
set(CMAKE_CXX_STANDARD ${MS_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(MS_DEBUG "Enable MediaSonic debug features" ON)
# Fancy banner like OpenXMB
string(ASCII 27 ESC)
set(C_RESET "${ESC}[0m")
set(C_GRN   "${ESC}[32m")
set(C_YEL   "${ESC}[33m")
set(C_CYN   "${ESC}[36m")
message(STATUS "${C_GRN}== MediaSonic Builder ==${C_RESET}")
message(STATUS "${C_YEL}TM & (C) 2025 Syndromatic Ltd. All rights reserved.${C_RESET}")
if(APPLE)
  set(_plat "Mac OS X")
elseif(WIN32)
  set(_plat "Windows")
elseif(UNIX)
  set(_plat "Linux/SynOS")
else()
  set(_plat "Unknown")
endif()
message(STATUS "Running on ${C_CYN}${_plat}${C_RESET}")
message(STATUS "Project: MediaSonic ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "MS_DEBUG: ${MS_DEBUG}")

# Find necessary Qt5 packages
find_package(Qt5 COMPONENTS Core Gui Widgets Multimedia MultimediaWidgets Concurrent Xml REQUIRED)
# Optional Qt modules used by Atmo (UNO/style)
find_package(Qt5DBus QUIET)
find_package(Qt5X11Extras QUIET)
list(APPEND CMAKE_MODULE_PATH "/usr/share/ECM/modules")
list(APPEND CMAKE_MODULE_PATH "/usr/share/ECM/find-modules")
find_package(KF5 COMPONENTS I18n CoreAddons WidgetsAddons KIO REQUIRED)

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
    # Core style helpers (inspired by DocSurf)
    src/gfx/color.cpp
    src/gfx/color.h
    src/gfx/fx.cpp
    src/gfx/fx.h
    src/ui/atmo_style.cpp
    src/ui/atmo_style.h
    src/ui/nse_uno.cpp
    src/ui/nse_uno.h
    # Models
    src/models/track.h
    src/models/trackmodel.cpp
    src/models/trackmodel.h
    # Services
    src/services/scanner.cpp
    src/services/scanner.h
    # Visualizer
    src/visualizer/visualizerbridge.cpp
    src/visualizer/visualizerbridge.h
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

# --- Atmo NSE integration (UNO + style) ---
# Prefer fetching from GitHub; allow override with -DATMO_DIR=... to use a local checkout.
option(ATMO_USE_FETCH "Fetch Atmo-Desktop with FetchContent" ON)
set(ATMO_GIT_URL "https://github.com/phenom64/Atmo-Desktop.git" CACHE STRING "Atmo-Desktop Git URL")
set(ATMO_GIT_TAG "master" CACHE STRING "Atmo-Desktop Git ref (branch/tag/sha)")

set(_HAVE_ATMO OFF)
set(_ATMO_SOURCE "")

if(ATMO_DIR AND EXISTS "${ATMO_DIR}/CMakeLists.txt")
  set(_HAVE_ATMO ON)
  set(_ATMO_SOURCE "${ATMO_DIR}")
  message(STATUS "Integrating Atmo from ATMO_DIR=${ATMO_DIR}")
elseif(ATMO_USE_FETCH)
  include(FetchContent)
  message(STATUS "Fetching Atmo-Desktop from ${ATMO_GIT_URL} (${ATMO_GIT_TAG})")
  FetchContent_Declare(
    atmo
    GIT_REPOSITORY ${ATMO_GIT_URL}
    GIT_TAG        ${ATMO_GIT_TAG}
    GIT_SHALLOW    OFF
    UPDATE_DISCONNECTED ON
  )
  # We only populate to control the build dir name; do not MakeAvailable
  FetchContent_GetProperties(atmo)
  if(NOT atmo_POPULATED)
    FetchContent_Populate(atmo)
  endif()
  set(_HAVE_ATMO ON)
  set(_ATMO_SOURCE ${atmo_SOURCE_DIR})
endif()

if(_HAVE_ATMO)
  if(Qt5DBus_FOUND)
    set(QTDBUS_FOUND ON)
  endif()
  # Build the Atmo project under a neat path in the build tree
  add_subdirectory("${_ATMO_SOURCE}" "${CMAKE_BINARY_DIR}/NSE/atmo")

  # Define source dir for runtime NSE.conf provisioning
  target_compile_definitions(MediaSonic PRIVATE MS_ATMO_SOURCE_DIR="$<IF:$<BOOL:${_ATMO_SOURCE}>,${_ATMO_SOURCE},>")
else()
  message(WARNING "Atmo-Desktop missing: define ATMO_DIR or enable ATMO_USE_FETCH to get UNO + style.")
endif()

# Link the necessary libraries
target_link_libraries(MediaSonic
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Multimedia
    Qt5::MultimediaWidgets
    Qt5::Concurrent
    Qt5::Xml
    # Optional Qt modules (safe to include even if not found)
    $<$<TARGET_EXISTS:Qt5::DBus>:Qt5::DBus>
    $<$<TARGET_EXISTS:Qt5::X11Extras>:Qt5::X11Extras>
    KF5::I18n
    KF5::CoreAddons
    KF5::WidgetsAddons
    KF5::KIOWidgets
    KF5::KIOFileWidgets
)

# Optional Taglib for metadata scanning
find_package(Taglib QUIET)
if(Taglib_FOUND)
    message(STATUS "Taglib found: enabling enhanced metadata scanning")
    target_compile_definitions(MediaSonic PRIVATE HAVE_TAGLIB)
    # Prefer imported target if available
    if(TARGET Taglib::Taglib)
        target_link_libraries(MediaSonic Taglib::Taglib)
    elseif(TARGET TagLib::TagLib)
        target_link_libraries(MediaSonic TagLib::TagLib)
    else()
        # Fallback to plain taglib if the module provides variables
        target_link_libraries(MediaSonic tag)
    endif()
endif()

# Installation path
install(TARGETS MediaSonic
    RUNTIME DESTINATION bin
)

# Copy runtime fonts to the build output for fallback loading
# Post-build dev assets (fonts) for run-in-place
set(_dev_dir "${CMAKE_CURRENT_BINARY_DIR}/share/mediasonic/fonts")
add_custom_command(TARGET MediaSonic POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:MediaSonic>/fonts"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${_dev_dir}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/src/fonts/DS-DIGII.TTF"
            "$<TARGET_FILE_DIR:MediaSonic>/fonts/DS-DIGII.TTF"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/src/fonts/DS-DIGII.TTF"
            "${_dev_dir}/DS-DIGII.TTF"
)

# Add uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
if(NOT TARGET uninstall)
  add_custom_target(uninstall
      COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
