add_definitions(-DTT_BUILDING_DLL)

if(NOT WIN32)
  add_compile_options(-Wno-deprecated-declarations)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)

message("Ribbon: Qt${QT_VERSION_MAJOR} found")
find_package(
        Qt${QT_VERSION_MAJOR}
        REQUIRED
        COMPONENTS
        Core
        Gui
        Widgets
        CONFIG
)

set(INCROOT ${PROJECT_SOURCE_DIR}/include/TabToolbar)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/TabToolbar)

set(SRC_LIST
    ${INCROOT}/API.h
    ${SRCROOT}/TabToolbar.cpp
    ${INCROOT}/TabToolbar.h
    ${SRCROOT}/Page.cpp
    ${INCROOT}/Page.h
    ${SRCROOT}/Builder.cpp
    ${INCROOT}/Builder.h
    ${SRCROOT}/CompactToolButton.cpp
    ${SRCROOT}/CompactToolButton.h
    ${SRCROOT}/Group.cpp
    ${INCROOT}/Group.h
    ${SRCROOT}/SubGroup.cpp
    ${INCROOT}/SubGroup.h
    ${SRCROOT}/StyleTools.cpp
    ${INCROOT}/StyleTools.h
    ${SRCROOT}/Styles.cpp
    ${INCROOT}/Styles.h
    ${SRCROOT}/ToolButtonStyle.cpp
    ${SRCROOT}/ToolButtonStyle.h
)
source_group("" FILES ${SRC_LIST})

set(RESOURCES
    ${SRCROOT}/StyleTemplate.qrc
)
source_group("resources" FILES ${RESOURCES})

add_library(TabToolbar SHARED ${SRC_LIST} ${RESOURCES})

set(QtLibList
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
)

target_link_libraries(TabToolbar
     ${QtLibList}
)

if(WIN32)
	# include the major version number in Windows shared library names (but not import library names)
	set_target_properties(TabToolbar PROPERTIES DEBUG_POSTFIX "d")
	set_target_properties(TabToolbar PROPERTIES SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
else()
	set_target_properties(TabToolbar PROPERTIES DEBUG_POSTFIX "d")
endif()
if (WIN32 AND MINGW)
	# on Windows/gcc get rid of "lib" prefix for shared libraries,
	# and transform the ".dll.a" suffix into ".a" for import libraries
	set_target_properties(TabToolbar PROPERTIES PREFIX "")
	set_target_properties(TabToolbar PROPERTIES IMPORT_SUFFIX ".a")
endif()

install(TARGETS TabToolbar
        RUNTIME DESTINATION bin COMPONENT bin
		LIBRARY DESTINATION lib COMPONENT bin
		ARCHIVE DESTINATION lib COMPONENT devel
		FRAMEWORK DESTINATION ${CMAKE_INSTALL_FRAMEWORK_PREFIX} COMPONENT bin
)
