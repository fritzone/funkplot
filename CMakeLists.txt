cmake_minimum_required(VERSION 3.10)

# set the project name
project(GCompiler)
set(project ggui)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/home/fld/Qt/6.3.0/gcc_64/lib/cmake")

find_package(QT NAMES Qt6
    COMPONENTS
        Widgets
        OpenGL

        3DCore
        3DRender
        3DInput
        3DLogic
        3DExtras
        3DAnimation
    REQUIRED
)

message(Qt${QT_VERSION_MAJOR} found)

set(QT_COMPONENTS Core Gui Widgets OpenGL 3DCore 3DRender 3DInput 3DLogic 3DExtras 3DAnimation)

set(KDDockWidgets_QT6 true)

list(APPEND QT_COMPONENTS OpenGLWidgets)

find_package(
    Qt${QT_VERSION_MAJOR}
        REQUIRED
        COMPONENTS
        ${QT_COMPONENTS}
    CONFIG
)

Include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.0.0-preview3
)

FetchContent_MakeAvailable(Catch2)

add_subdirectory(KDDockWidgets)
add_subdirectory(ribbon)


add_library(funplot STATIC
    Function.cpp Function.h
    RuntimeProvider.cpp RuntimeProvider.h
    util.cpp util.h
    CodeEngine.cpp CodeEngine.h

    Keywords.h
    Statement.h
    Stepped.h
    Set.h
    Executor.h

    constants.h
)

target_link_libraries(funplot
    Qt${QT_VERSION_MAJOR}::Core
)

add_executable(tests compiler.cpp
)
target_link_libraries(tests
    PRIVATE
      Catch2::Catch2WithMain
      funplot
    PUBLIC
      Qt${QT_VERSION_MAJOR}::Core
      Qt${QT_VERSION_MAJOR}::Widgets
      Qt${QT_VERSION_MAJOR}::Gui
)

include_directories(
    ${catch2_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/KDDockWidgets
    ${CMAKE_SOURCE_DIR}/KDDockWidgets/src
    ${CMAKE_SOURCE_DIR}/ribbon/include
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)


if(${QT_VERSION_MAJOR} STREQUAL "6")
  qt6_wrap_ui(GUI_UI_FILES MainWindow.ui)
  qt6_add_resources(GUI_QRC_FILES main.qrc)
else()
  qt5_wrap_ui(GUI_UI_FILES MainWindow.ui)
  qt5_add_resources(GUI_QRC_FILES main.qrc)
endif()

add_executable(ggui WIN32
    main.cpp
    AbstractDrawer.cpp AbstractDrawer.h
    Highlighter.h Highlighter.cpp
    MainWindow.h MainWindow.cpp MainWindow.ui
    MyGraphicsView.h MyGraphicsView.cpp
    GraphicsViewZoom.h GraphicsViewZoom.cpp
    FrameForLineNumbers.h FrameForLineNumbers.cpp
    TextEditWithCodeCompletion.h TextEditWithCodeCompletion.cpp
    ListWidgetForCodeCompletion.h ListWidgetForCodeCompletion.cpp
    DrawingForm.h DrawingForm.cpp DrawingForm.ui
    PlotRenderer.h PlotRenderer.cpp
    CodeEditorTabPage.h CodeEditorTabPage.cpp CodeEditorTabPage.ui
    ProjectedRenderer.h ProjectedRenderer.cpp

    ${GUI_UI_FILES}
    ${GUI_QRC_FILES}
)

set(QtLibList
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::OpenGL

    Qt${QT_VERSION_MAJOR}::3DCore
    Qt${QT_VERSION_MAJOR}::3DRender
    Qt${QT_VERSION_MAJOR}::3DLogic
    Qt${QT_VERSION_MAJOR}::3DExtras
    Qt${QT_VERSION_MAJOR}::3DAnimation
)
if(${QT_VERSION_MAJOR} STREQUAL "6")
    list(APPEND QtLibList Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
endif()


find_package(OpenGL REQUIRED)

target_link_libraries(ggui
    PUBLIC
    ${QtLibList}
    PRIVATE
    funplot
    kddockwidgets
    TabToolbar
    ${OPENGL_LIBRARIES}
)

if(WIN32)

  if(Qt${QT_VERSION_MAJOR}_FOUND AND TARGET Qt${QT_VERSION_MAJOR}::qmake AND NOT TARGET Qt${QT_VERSION_MAJOR}::windeployqt)
      get_target_property(_qt${QT_VERSION_MAJOR}_qmake_location
          Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION
      )

      execute_process(
          COMMAND "${_qt${QT_VERSION_MAJOR}_qmake_location}" -query QT_INSTALL_PREFIX
          RESULT_VARIABLE return_code
          OUTPUT_VARIABLE qt${QT_VERSION_MAJOR}_install_prefix
          OUTPUT_STRIP_TRAILING_WHITESPACE
      )

      set(imported_location "${qt${QT_VERSION_MAJOR}_install_prefix}/bin/windeployqt.exe")

      if(EXISTS ${imported_location})
          add_executable(Qt${QT_VERSION_MAJOR}::windeployqt IMPORTED)

          set_target_properties(Qt${QT_VERSION_MAJOR}::windeployqt PROPERTIES
              IMPORTED_LOCATION ${imported_location}
          )
      endif()
  endif()

   if(TARGET Qt${QT_VERSION_MAJOR}::windeployqt)
        add_custom_command(TARGET ${project}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/bin/package"
            COMMAND set PATH=%PATH%$<SEMICOLON>${qt${QT_VERSION_MAJOR}_install_prefix}/bin
            COMMAND Qt${QT_VERSION_MAJOR}::windeployqt --dir "${CMAKE_BINARY_DIR}/bin/package" "$<TARGET_FILE:${project}>"
        )

        add_custom_command(TARGET ${project}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${project}> "${CMAKE_BINARY_DIR}/bin/package"
        )

        add_custom_command(TARGET ${project}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kddockwidgets> "${CMAKE_BINARY_DIR}/bin/package"
        )

        add_custom_command(TARGET ${project}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:TabToolbar> "${CMAKE_BINARY_DIR}/bin/package"
        )

        add_custom_command(TARGET ${project}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/License "${CMAKE_BINARY_DIR}/bin/package"
        )

    else()
        message(FATAL_ERROR "Could not find windeployqt")
    endif()

endif()




####################################################################################################
# Debian packaging stuff                                                                           #
####################################################################################################

if(NOT WIN32 AND FALSE)

# SET(CPACK_GENERATOR "DEB")
set(DRV2WEB_CLIENT_BIN_DIR "bin")

# Project Info
SET(PROJECT_NAME_SHORT   "cloudysh")
SET(PROJECT_NAME_LONG    "cloudy.sh client")
SET(PROJECT_DESCRIPTION  "cloudy.sh client - the client application of cloudy.sh")
SET(PROJECT_COPYRIGHT    "Copyright (c) 2016 The Unauthorized Frog, Copyright (c) 2016 cloudy.sh")
SET(PROJECT_CONTACT      "contact@cloudy.sh")
SET(PROJECT_VENDOR       "cloudy.sh")
SET(ORG_WEBSITE          "https://cloudy.sh/")

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_SYSTEM_NAME                 "${CMAKE_SYSTEM_NAME}")
SET(CPACK_PACKAGE_NAME                "cloudysh-client")
SET(CPACK_PACKAGE_DESCRIPTION         "${PROJECT_DESCRIPTION}")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Client for the cloudy.sh network. Allows you to perform standard operations, such as as serving albums and sharing folders.")
SET(CPACK_PACKAGE_VENDOR              "${PROJECT_VENDOR}")
SET(CPACK_PACKAGE_CONTACT             "${PROJECT_CONTACT}")
SET(CPACK_PACKAGE_DESCRIPTION_FILE    "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_README        "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE       "${CMAKE_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY   "${CMAKE_CURRENT_BINARY_DIR}")
set(CPACK_PACKAGE_VERSION             "${CLIENT_VERSION_MAJOR}.${CLIENT_VERSION_MINOR}.${CLIENT_VERSION_PATCH}")
set(CPACK_PACKAGE_RELEASE             1)
set(CPACK_PACKAGING_INSTALL_PREFIX    ${CMAKE_INSTALL_PREFIX})

# DEB specific

SET(CPACK_DEBIAN_PACKAGE_DEPENDS      "libqt5widgets5, libqt5core5a, libqt5gui5, libqt5network5, libqt5widgets5, zlib1g, libc6, libqt5widgets5, 
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

if(BUILD_SNAP)
  INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/qtclient/cloudysh-client DESTINATION ${DRV2WEB_CLIENT_BIN_DIR})
else()
  INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/bin/cloudysh-client DESTINATION ${DRV2WEB_CLIENT_BIN_DIR})
endif()
INSTALL(FILES "qtclient/cloudysh-client.desktop" DESTINATION "/usr/share/applications/")
install(FILES "qtclient/icons/hicolor/48x48/cloudysh-client.png"   DESTINATION "/usr/share/icons/hicolor/48x48/apps/")
install(FILES "qtclient/icons/hicolor/256x256/cloudysh-client.png" DESTINATION "/usr/share/icons/hicolor/256x256/apps/")

SET(CPACK_PACKAGE_EXECUTABLES "cloudysh-client")

INCLUDE(CPack)

endif()

## Debian packaging stuff ends here
####################################################################################################
