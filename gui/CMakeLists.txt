set(PROJECT funkplot-gui)
project(${PROJECT})

find_package(OpenGL REQUIRED)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/ggui.rc")

# Generate the builtin qrc only when its inputs change, not on every compiler rebuild
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/builtins.qrc
    DEPENDS
        ${CMAKE_SOURCE_DIR}/gui/2dcurves.json
        ${CMAKE_SOURCE_DIR}/scripts/builtins.py
    COMMAND python ../scripts/builtins.py
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(funkplot-builtins DEPENDS ${CMAKE_BINARY_DIR}/builtins.qrc)
# Compiler must exist before the script runs, but rebuilding it doesn't retrigger generation
add_dependencies(funkplot-builtins funkplot-compiler)

set(${PROJECT}_srcs
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AboutDialog.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AboutDialog.h
	${CMAKE_CURRENT_SOURCE_DIR}/AboutDialog.ui
	${CMAKE_CURRENT_SOURCE_DIR}/Builtin.h
	${CMAKE_CURRENT_SOURCE_DIR}/Builtin.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/BuiltinFunctionDetailer.h
	${CMAKE_CURRENT_SOURCE_DIR}/BuiltinFunctionDetailer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/BuiltinFunctionDetailer.ui
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
	${CMAKE_CURRENT_SOURCE_DIR}/MouseSensitiveLabel.h
	${CMAKE_CURRENT_SOURCE_DIR}/MouseSensitiveLabel.cpp
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
	${CMAKE_BINARY_DIR}/builtins.qrc

	${APP_ICON_RESOURCE_WINDOWS}
)

qt5_add_resources(HELP_QRC ${CMAKE_BINARY_DIR}/help/help.qrc)
list(APPEND ${PROJECT}_srcs ${HELP_QRC})

if(USE_QT6)
	list(APPEND ${PROJECT}_srcs
	${CMAKE_CURRENT_SOURCE_DIR}/ProjectedRenderer.h
	${CMAKE_CURRENT_SOURCE_DIR}/ProjectedRenderer.cpp)
endif()

list(APPEND ${PROJECT}_srcs
	${CMAKE_CURRENT_SOURCE_DIR}/SystemSettings.h
	${CMAKE_CURRENT_SOURCE_DIR}/SystemSettings.cpp)

if(WIN32)
	add_executable(funkplot-gui WIN32 ${${PROJECT}_srcs} )
else()
	add_executable(funkplot-gui	${${PROJECT}_srcs})
endif()

add_dependencies(funkplot-gui funkplot-help)

if(ENABLE_PYTHON)
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

        set(PACKAGE_DIR "${CMAKE_BINARY_DIR}/bin/package")

        if(ENABLE_PYTHON)
            # funkplot-gui embeds the interpreter, so the package has to carry the
            # Python runtime itself or the program will not even start: the DLL, the
            # standard library and the extension modules. Lib/ and DLLs/ go next to
            # the executable because that is the layout Py_Initialize() probes for
            # (Lib/os.py is the landmark that makes the package directory the
            # prefix), and pythonXY._pth beside the DLL pins sys.path to exactly
            # those two directories, so a PYTHONHOME or PYTHONPATH on the target
            # machine cannot redirect the embedded interpreter elsewhere.
            find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

            get_filename_component(PYTHON_RUNTIME_DIR "${Python3_EXECUTABLE}" DIRECTORY)
            set(PYTHON_VER_TAG "${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
            set(PYTHON_DLL "${PYTHON_RUNTIME_DIR}/python${PYTHON_VER_TAG}.dll")

            foreach(required_part "${PYTHON_DLL}" "${PYTHON_RUNTIME_DIR}/Lib/os.py" "${PYTHON_RUNTIME_DIR}/DLLs")
                if(NOT EXISTS "${required_part}")
                    message(FATAL_ERROR
                        "Cannot deploy the embedded Python runtime: ${required_part} is missing. "
                        "Point Python3_EXECUTABLE at a regular CPython installation for Windows, "
                        "or configure with -DENABLE_PYTHON=OFF.")
                endif()
            endforeach()

            set(PYTHON_PTH "${CMAKE_CURRENT_BINARY_DIR}/python${PYTHON_VER_TAG}._pth")
            file(WRITE "${PYTHON_PTH}" "Lib\nDLLs\n.\n")

            # python3.dll is not linked by funkplot itself, but extension modules in
            # DLLs/ that were built against the stable ABI need it.
            set(PYTHON_RUNTIME_FILES "\"${PYTHON_DLL}\" \"${PYTHON_PTH}\"")
            if(EXISTS "${PYTHON_RUNTIME_DIR}/python3.dll")
                string(APPEND PYTHON_RUNTIME_FILES " \"${PYTHON_RUNTIME_DIR}/python3.dll\"")
            endif()

            set(PYTHON_DEPLOY_STEP
"file(COPY ${PYTHON_RUNTIME_FILES} DESTINATION \"${PACKAGE_DIR}\")
file(COPY \"${PYTHON_RUNTIME_DIR}/Lib\" \"${PYTHON_RUNTIME_DIR}/DLLs\" DESTINATION \"${PACKAGE_DIR}\")
")
        else()
            set(PYTHON_DEPLOY_STEP "")
        endif()

        # The whole package is assembled by one generated script instead of a chain
        # of POST_BUILD commands. Ninja folds all post-build steps of a target into a
        # single cmd.exe invocation, and with this machine's PATH expanded into the
        # `set PATH=%PATH%;...` step that chain sat a few characters below cmd's 8191
        # character limit - adding anything to it failed with "The input line is too
        # long". A script keeps the command line short and the steps readable.
        set(DEPLOY_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/deploy_package_$<CONFIG>.cmake")

        file(GENERATE OUTPUT "${DEPLOY_SCRIPT}" CONTENT
"# Generated by gui/CMakeLists.txt. Assembles bin/package for the installer.
file(REMOVE_RECURSE \"${PACKAGE_DIR}\")
file(MAKE_DIRECTORY \"${PACKAGE_DIR}\")

set(ENV{PATH} \"${qt${QT_VERSION_MAJOR}_install_prefix}/bin;\$ENV{PATH}\")
execute_process(
    COMMAND \"$<TARGET_FILE:Qt${QT_VERSION_MAJOR}::windeployqt>\" --dir \"${PACKAGE_DIR}\" \"$<TARGET_FILE:${PROJECT}>\"
    RESULT_VARIABLE windeployqt_result
)
if(NOT windeployqt_result EQUAL 0)
    message(FATAL_ERROR \"windeployqt failed: \${windeployqt_result}\")
endif()

file(COPY
    \"$<TARGET_FILE:${PROJECT}>\"
    \"$<TARGET_FILE:kddockwidgets>\"
    \"$<TARGET_FILE:TabToolbar>\"
    DESTINATION \"${PACKAGE_DIR}\")

# setup.64.iss takes its LicenseFile from the package directory under this name.
configure_file(\"${CMAKE_SOURCE_DIR}/LICENSE\" \"${PACKAGE_DIR}/LICENSE.TXT\" COPYONLY)

# On Windows HelpWindow opens the collection from <app dir>/help/funkplot.qhc, so
# the collection ships beside the executable rather than only inside the resources
# the way the Linux build consumes it. funkplot.qch has to travel with it: the
# collection registers it by plain file name, relative to the collection itself.
file(COPY
    \"${CMAKE_BINARY_DIR}/help/funkplot.qhc\"
    \"${CMAKE_BINARY_DIR}/help/funkplot.qch\"
    DESTINATION \"${PACKAGE_DIR}/help\")

${PYTHON_DEPLOY_STEP}")

        add_custom_command(TARGET ${PROJECT}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -P "${DEPLOY_SCRIPT}"
            COMMENT "Assembling bin/package with windeployqt"
        )

    else()
        message(FATAL_ERROR "Could not find windeployqt")
    endif()

endif()
