cmake_minimum_required(VERSION 3.10)

# set the project name
project(GCompiler)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)

message(Qt${QT_VERSION_MAJOR} found)


if(WIN32)
    set(DKDDockWidgets_QT6 ON)

    find_package(
        Qt${QT_VERSION_MAJOR}
        REQUIRED
        COMPONENTS
        Core
        Gui
        Widgets
        CONFIG
)
else()
find_package(
    Qt${QT_VERSION_MAJOR}
    REQUIRED
    COMPONENTS
    Core
    Gui
    Widgets
    X11Extras
    CONFIG
)
endif()

Include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.0.0-preview3
)

FetchContent_MakeAvailable(Catch2)

add_subdirectory(KDDockWidgets)
find_package(KDDockWidgets CONFIG PATHS ${CMAKE_SOURCE_DIR}/KDDockWidgets)

add_library(funplot STATIC
    Function.cpp Function.h
    RuntimeProvider.cpp RuntimeProvider.h
    util.cpp util.h
    CodeEngine.cpp CodeEngine.h
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

add_executable(ggui
    main.cpp
    Highlighter.h Highlighter.cpp
    MainWindow.h MainWindow.cpp MainWindow.ui
    MyGraphicsView.h MyGraphicsView.cpp
    GraphicsViewZoom.h GraphicsViewZoom.cpp
    FrameForLineNumbers.h FrameForLineNumbers.cpp
    TextEditWithCodeCompletion.h TextEditWithCodeCompletion.cpp
    ListWidgetForCodeCompletion.h ListWidgetForCodeCompletion.cpp
    DrawingForm.h DrawingForm.cpp DrawingForm.ui
    ${GUI_UI_FILES}
    ${GUI_QRC_FILES}
)

set(QtLibList
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
)

if(NOT WIN32)
    LIST(APPEND QtLibList Qt${QT_VERSION_MAJOR}::X11Extras)
endif()

target_link_libraries(ggui
    PUBLIC
    ${QtLibList}
    PRIVATE
    funplot
    kddockwidgets
)
