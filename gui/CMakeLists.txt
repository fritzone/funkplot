set(PROJECT funkplot-gui)
project(${PROJECT})

find_package(OpenGL REQUIRED)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/ggui.rc")

set(${PROJECT}_srcs
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AboutDialog.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AboutDialog.h
	${CMAKE_CURRENT_SOURCE_DIR}/AboutDialog.ui
	${CMAKE_CURRENT_SOURCE_DIR}/ClickableLabel.h
	${CMAKE_CURRENT_SOURCE_DIR}/ClickableLabel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CodeEditorTabPage.h
	${CMAKE_CURRENT_SOURCE_DIR}/CodeEditorTabPage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CodeEditorTabPage.ui
	${CMAKE_CURRENT_SOURCE_DIR}/DrawingForm.h
	${CMAKE_CURRENT_SOURCE_DIR}/DrawingForm.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DrawingForm.ui
	${CMAKE_CURRENT_SOURCE_DIR}/ExportImageDialog.h
	${CMAKE_CURRENT_SOURCE_DIR}/ExportImageDialog.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ExportImageDialog.ui
	${CMAKE_CURRENT_SOURCE_DIR}/FrameForLineNumbers.h
	${CMAKE_CURRENT_SOURCE_DIR}/FrameForLineNumbers.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/GraphicsViewZoom.h
	${CMAKE_CURRENT_SOURCE_DIR}/GraphicsViewZoom.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/HelpWindow.h
	${CMAKE_CURRENT_SOURCE_DIR}/HelpWindow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/HelpWindow.ui
	${CMAKE_CURRENT_SOURCE_DIR}/Highlighter.h
	${CMAKE_CURRENT_SOURCE_DIR}/Highlighter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ListWidgetForCodeCompletion.h
	${CMAKE_CURRENT_SOURCE_DIR}/ListWidgetForCodeCompletion.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.h
	${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.ui
	${CMAKE_CURRENT_SOURCE_DIR}/MyGraphicsView.h
	${CMAKE_CURRENT_SOURCE_DIR}/MyGraphicsView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/TextEditWithCodeCompletion.h
	${CMAKE_CURRENT_SOURCE_DIR}/TextEditWithCodeCompletion.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PaletteListForm.h
	${CMAKE_CURRENT_SOURCE_DIR}/PaletteListForm.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PaletteListForm.ui
	${CMAKE_CURRENT_SOURCE_DIR}/PlotRenderer.h
	${CMAKE_CURRENT_SOURCE_DIR}/PlotRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Program.h
	${CMAKE_CURRENT_SOURCE_DIR}/Program.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/QtHelpSchemeHandler.h
	${CMAKE_CURRENT_SOURCE_DIR}/QtHelpSchemeHandler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ScreenDrawer.h
	${CMAKE_CURRENT_SOURCE_DIR}/ScreenDrawer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ViewType.h

	${CMAKE_CURRENT_SOURCE_DIR}/main.qrc

	${APP_ICON_RESOURCE_WINDOWS}
)

if(USE_QT6)
	list(APPEND ${PROJECT}_srcs
	${CMAKE_CURRENT_SOURCE_DIR}/ProjectedRenderer.h
	${CMAKE_CURRENT_SOURCE_DIR}/ProjectedRenderer.cpp)
endif()

if(WIN32)
	add_executable(funkplot-gui WIN32 ${${PROJECT}_srcs} )
else()
	add_executable(funkplot-gui	${${PROJECT}_srcs})
endif()

if(ENABLE_PYTHON)

	if(USE_UNITTEST)
		target_compile_definitions(tests PRIVATE "ENABLE_PYTHON")
	endif()

	target_compile_definitions(${PROJECT} PRIVATE "ENABLE_PYTHON")
endif()

message("Libs: ${QtLibList}")

target_link_libraries(funkplot-gui
	PRIVATE
	funkplot
	kddockwidgets
	TabToolbar
	${QtLibList}
	${OPENGL_LIBRARIES}
)

target_include_directories(${PROJECT} PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(${PROJECT} PRIVATE ${CMAKE_SOURCE_DIR})

set(FUNKPLOT_CLIENT_BIN_DIR ${CMAKE_INSTALL_BINDIR})

INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT} DESTINATION ${FUNKPLOT_CLIENT_BIN_DIR})

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
        add_custom_command(TARGET ${PROJECT}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/bin/package"
            COMMAND set PATH=%PATH%$<SEMICOLON>${qt${QT_VERSION_MAJOR}_install_prefix}/bin
            COMMAND Qt${QT_VERSION_MAJOR}::windeployqt --dir "${CMAKE_BINARY_DIR}/bin/package" "$<TARGET_FILE:${PROJECT}>"
        )

        add_custom_command(TARGET ${PROJECT}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT}> "${CMAKE_BINARY_DIR}/bin/package"
        )

        add_custom_command(TARGET ${PROJECT}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kddockwidgets> "${CMAKE_BINARY_DIR}/bin/package"
        )

        add_custom_command(TARGET ${PROJECT}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:TabToolbar> "${CMAKE_BINARY_DIR}/bin/package"
        )

        add_custom_command(TARGET ${PROJECT}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/License "${CMAKE_BINARY_DIR}/bin/package"
        )

    else()
        message(FATAL_ERROR "Could not find windeployqt")
    endif()

endif()
