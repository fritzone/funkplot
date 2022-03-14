cmake_minimum_required(VERSION 3.10)

# set the project name
project(GCompiler)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(
	Qt5
	REQUIRED
	COMPONENTS
		Core
		Gui
		Widgets
	CONFIG
	)


Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3
)

FetchContent_MakeAvailable(Catch2)

# add the executable
add_library(gcompi compiler.cpp Function.cpp RuntimeProvider.cpp)
target_link_libraries(gcompi Qt5::Core)

add_executable(tests compiler.cpp)
target_link_libraries(tests
    PRIVATE Catch2::Catch2WithMain
    PUBLIC gcompi)

include_directories(${catch2_SOURCE_DIR}/src)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)

qt5_wrap_ui(GUI_UI_FILES MainWindow.ui)
qt5_add_resources(GUI_QRC_FILES main.qrc)

add_executable(ggui
		main.cpp
		MainWindow.h MainWindow.cpp MainWindow.ui

		${GUI_UI_FILES}
		${GUI_QRC_FILES}
)

target_link_libraries(ggui
	PUBLIC
		Qt5::Core
		Qt5::Widgets
		Qt5::Gui
	PRIVATE
		gcompi
)
