cmake_minimum_required(VERSION 3.10)

set(PROJECT tests)
project(${PROJECT})

Include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.1.0
)

FetchContent_MakeAvailable(Catch2)

set(${PROJECT}_srcs
   ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
)
add_executable(${PROJECT} ${${PROJECT}_srcs})

target_link_libraries(${PROJECT}
    PRIVATE
      Catch2::Catch2WithMain
      funkplot
    PUBLIC
      Qt${QT_VERSION_MAJOR}::Core
      Qt${QT_VERSION_MAJOR}::Widgets
      Qt${QT_VERSION_MAJOR}::Gui
)

target_include_directories(${PROJECT} 
  PRIVATE 
  ${CMAKE_SOURCE_DIR}/lib
  ${CMAKE_SOURCE_DIR}
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

catch_discover_tests(${PROJECT})
