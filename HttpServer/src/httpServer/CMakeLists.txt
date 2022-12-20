cmake_minimum_required(VERSION 3.6)

project(httpServer LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

set(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/httpConnection.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpData.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpRequest.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpRequestRouter.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpResponse.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpServer.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/middleware/CORS.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/middleware/auth.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/middleware/getArray.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/middleware/getObject.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/middleware/verifyJson.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/util.cpp


        ${CMAKE_CURRENT_SOURCE_DIR}/const.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpConnection.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpCookie.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpData.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpRequest.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpRequestHandler.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpRequestRouter.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpResponse.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpServer.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/httpServerConfig.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/middleware.h 
        ${CMAKE_CURRENT_SOURCE_DIR}/util.h
)

add_library(httpServer STATIC ${SOURCE_FILES})

target_link_libraries(httpServer
	Qt5::Core Qt5::Network
)
