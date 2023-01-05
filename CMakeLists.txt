cmake_minimum_required(VERSION 3.10)

project(funkplot)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.16.0") 
  cmake_policy(VERSION 3.16)
  cmake_policy(SET CMP0072 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_WARN_DEPRECATED OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(BUILD_GUI "Build the GUI of the application" ON)
option(BUILD_COMPILER "Build the compiler of the application" OFF)
option(BUILD_WEB "Build the WEB part of the application" OFF)
option(USE_QT6 "Use Qt6 for building" OFF)
option(USE_UNITTEST "Build some unittesting too" OFF)
option(ENABLE_PYTHON "Enabled running pythons scripts" ON)

if(SNAP_BUILD)
  add_definitions("-DSNAP_BUILD")
endif()

if(HELP_TARGET_DIR)
	message("Target help dir: ${HELP_TARGET_DIR}")
	add_definitions("-DHELP_TARGET_DIR=${HELP_TARGET_DIR}")
endif()

if(BUILD_GUI AND BUILD_WEB)
	message( FATAL_ERROR "Cannot build both WEB and GUI in the same run, turn off one of those")
endif()

if(USE_QT6)
  if(NOT WIN32)
    set(CMAKE_PREFIX_PATH "/home/fld/Qt/6.3.0/gcc_64/lib/cmake")
  else()
    set(CMAKE_PREFIX_PATH "d:/work/Qt6/6.2.1/msvc2019_64/lib/cmake")
  endif()  

  find_package(QT NAMES Qt6
    COMPONENTS
        Widgets
        OpenGL
        Help
        WebEngineWidgets
        3DCore
        3DRender
        3DInput
        3DLogic
        3DExtras
        3DAnimation
    REQUIRED
  )

  set(QT_COMPONENTS 
    Core
    Gui
    Widgets
    Help 
    OpenGL 
    WebEngineWidgets
	  3DCore 
    3DRender 
    3DInput 
    3DLogic 
    3DExtras 
    3DAnimation
  )

  set(KDDockWidgets_QT6 ON CACHE BOOL "Build KDDockWidgets against Qt 6" FORCE)

  list(APPEND QT_COMPONENTS OpenGLWidgets)

else()

  if(WIN32)
    set(CMAKE_PREFIX_PATH "D:/Qt/5.15.2/msvc2019_64")
  endif()  


  set(QT_COMPONENTS 
    Core
    Gui
  )  

  if(BUILD_GUI)

    list(APPEND QT_COMPONENTS 
        Widgets
        OpenGL
        Help
        WebEngineWidgets
        WebEngineCore
    )
  endif()

  find_package(QT NAMES Qt5
    COMPONENTS
    ${QT_COMPONENTS}
    REQUIRED
  )

endif()

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache) # Less useful to do it for linking, see edit2
endif(CCACHE_FOUND)

message(Qt${QT_VERSION_MAJOR}: found)

find_package(
    Qt${QT_VERSION_MAJOR}
        REQUIRED
        COMPONENTS
        ${QT_COMPONENTS}
    CONFIG
)

include_directories(
    ${catch2_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/KDDockWidgets
    ${CMAKE_SOURCE_DIR}/KDDockWidgets/src
    ${CMAKE_SOURCE_DIR}/ribbon/include
    ${CMAKE_SOURCE_DIR}/lib
)

set(QtLibList
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::Help
    Qt${QT_VERSION_MAJOR}::WebEngineWidgets
    Qt${QT_VERSION_MAJOR}::WebEngineCore
)

if(${QT_VERSION_MAJOR} STREQUAL "6")
    list(APPEND QtLibList 
        Qt${QT_VERSION_MAJOR}::OpenGLWidgets 
        Qt${QT_VERSION_MAJOR}::3DCore
        Qt${QT_VERSION_MAJOR}::3DRender
        Qt${QT_VERSION_MAJOR}::3DLogic
        Qt${QT_VERSION_MAJOR}::3DExtras
        Qt${QT_VERSION_MAJOR}::3DAnimation
    )
endif()

if(BUILD_GUI)
  add_subdirectory(KDDockWidgets)
  add_subdirectory(ribbon)
endif()

if(NOT WIN32)

  if(BUILD_GUI)
    add_subdirectory(help)
  endif()

  if(BUILD_COMPILER)
    add_subdirectory(compiler)
  endif()

endif()

if(BUILD_GUI)
  add_subdirectory(gui)
endif()

if(BUILD_WEB)
	set(ENABLE_PYTHON FALSE)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps")

add_subdirectory(lib)

if(USE_UNITTEST)
  add_subdirectory(test)
endif()

if(BUILD_WEB)
  include_directories(${CMAKE_SOURCE_DIR})
  add_subdirectory(HttpServer)
  add_subdirectory(web)
endif()


if(BUILD_GUI)

####################################################################################################
# Debian packaging stuff                                                                           #
####################################################################################################

if(NOT WIN32)

# Project Info
SET(PROJECT_NAME_SHORT   "fũnkplot")
SET(PROJECT_NAME_LONG    "fũnkplot")
SET(PROJECT_DESCRIPTION  "fũnkplot - A mathematical drawing and plotting application")
SET(PROJECT_COPYRIGHT    "Copyright (c) 2022 - 2023 The Unauthorized Frog")
SET(PROJECT_CONTACT      "fritzone@gmail.com")
SET(PROJECT_VENDOR       "The Unauthorized Frog")
SET(ORG_WEBSITE          "https://funkplot.sh/")

INCLUDE(InstallRequiredSystemLibraries)
include(GNUInstallDirs)

SET(CPACK_SYSTEM_NAME                 "${CMAKE_SYSTEM_NAME}")
SET(CPACK_PACKAGE_NAME                "funkplot")
SET(CPACK_PACKAGE_DESCRIPTION         "${PROJECT_DESCRIPTION}")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A mathematical drawing and plotting application")
SET(CPACK_PACKAGE_VENDOR              "${PROJECT_VENDOR}")
SET(CPACK_PACKAGE_CONTACT             "${PROJECT_CONTACT}")
SET(CPACK_PACKAGE_DESCRIPTION_FILE    "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_README        "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE       "${CMAKE_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY   "${CMAKE_CURRENT_BINARY_DIR}")
set(CLIENT_VERSION_MAJOR              1)
set(CLIENT_VERSION_MINOR              0)
set(CPACK_PACKAGE_VERSION             "${CLIENT_VERSION_MAJOR}.${CLIENT_VERSION_MINOR}")
set(CPACK_PACKAGE_RELEASE             1)
set(CPACK_PACKAGING_INSTALL_PREFIX    ${CMAKE_INSTALL_PREFIX})

# DEB specific

SET(CPACK_DEBIAN_PACKAGE_DEPENDS   "libqt5widgets5, libqt5core5a, libqt5gui5, libqt5network5, libqt5widgets5, zlib1g, libc6, libqt5widgets5, 
                                    libqt5core5a, libqt5gui5, libqt5network5, libqt5widgets5, libssl1.0.0, libxml2, libmodbus5, libev4, 
                                    libaec0, libhdf4-0-alt, libsz2, libexiv2-14, libsdl2-2.0-0, libqt5gui5, libqt5qml5, libqt5quick5, 
                                    libqt5widgets5, libqt5network5, libqt5multimedia5, libqt5script5, libqt5opengl5, libqt5serialport5, 
                                    libpolyclipping-dev, libhdf5-serial-dev, libqt5x11extras5, libxcb-xinerama0, libqt5sql5, libqt5xml5, libqt5sql5-sqlite, libcurl")
SET(CPACK_DEBIAN_PACKAGE_PRIORITY     "optional")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER   "${PROJECT_CONTACT}")


# RPM specific
# configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cloudysh-client.spec.in" "${CMAKE_CURRENT_BINARY_DIR}/cloudysh-client.spec" @ONLY IMMEDIATE)
# set(CPACK_RPM_USER_BINARY_SPECFILE "${CMAKE_CURRENT_BINARY_DIR}/cloudysh-client.spec")

# Force Package Name
SET(CPACK_PACKAGE_FILE_NAME
  ${CPACK_PACKAGE_NAME}_${CLIENT_VERSION_MAJOR}.${CLIENT_VERSION_MINOR}.${CLIENT_VERSION_PATCH}_${CMAKE_SYSTEM_PROCESSOR}
)
message("Package name will be: ${CPACK_PACKAGE_FILE_NAME}")

INSTALL(FILES "debian/funkplot-gui.desktop"                   DESTINATION "${CMAKE_INSTALL_DATADIR}/applications/")
install(FILES "debian/icons/hicolor/48x48/funkplot-gui.png"   DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps/")
install(FILES "debian/icons/hicolor/256x256/funkplot-gui.png" DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps/")
install(FILES "debian/funkplot.xml"                           DESTINATION "${CMAKE_INSTALL_DATADIR}/mime/application")

find_program(XDG-MIME_EXECUTABLE xdg-mime)
find_program(XDG-DESKTOP-MENU_EXECUTABLE xdg-desktop-menu)
find_program(XDG-ICON_RESOURCE xdg-icon-resource)

message("${XDG-MIME_EXECUTABLE} install --novendor ${CMAKE_SOURCE_DIR}/debian/funkplot.xml")
message("${XDG-DESKTOP-MENU_EXECUTABLE} install --novendor ${CMAKE_SOURCE_DIR}/debian/funkplot-gui.desktop")
message("${XDG-MIME_EXECUTABLE} default ${CMAKE_SOURCE_DIR}/debian/funkplot-gui.desktop application/x-funkplot-item")
message("${XDG-ICON_RESOURCE} install --context mimetype --novendor --size 256 --mode user ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps/funkplot-gui.png")

install(CODE "execute_process(COMMAND ${XDG-ICON_RESOURCE} install --context mimetype --novendor --size 256 --mode user ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps/funkplot-gui.png)" )
install(CODE "execute_process(COMMAND ${XDG-MIME_EXECUTABLE} install --novendor ${CMAKE_SOURCE_DIR}/debian/funkplot.xml)" )
install(CODE "execute_process(COMMAND ${XDG-DESKTOP-MENU_EXECUTABLE} install --novendor ${CMAKE_SOURCE_DIR}/debian/funkplot-gui.desktop)" )
install(CODE "execute_process(COMMAND ${XDG-MIME_EXECUTABLE} default ${CMAKE_SOURCE_DIR}/debian/funkplot-gui.desktop application/x-funkplot-item)" )

SET(CPACK_PACKAGE_EXECUTABLES "funkplot")

INCLUDE(CPack)

endif()

####################################################################################################
## Debian packaging stuff ends here
####################################################################################################

endif()
