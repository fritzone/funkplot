cmake_minimum_required(VERSION 3.6)
set(PROJECT_NAME funkplot-web)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set (CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Core Network Gui REQUIRED)
include_directories(
    ${CMAKE_SOURCE_DIR}/HttpServer/src/
)

set(SOURCE_FILES

    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/RequestHandler.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/RequestHandler.h

	${CMAKE_SOURCE_DIR}/compiler/compiler.qrc

	${CMAKE_CURRENT_SOURCE_DIR}/web.qrc

)


add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} 
	Qt5::Core Qt5::Network Qt5::Gui
	httpServer z funkplot
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
