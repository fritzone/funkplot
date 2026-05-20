CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

set(HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")

# New: Generate functions.html directly
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/functions.html
  COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/gen_function_db.py "${CMAKE_SOURCE_DIR}/gui/2dcurves.json" "${CMAKE_CURRENT_BINARY_DIR}/functions.html" "${CMAKE_BINARY_DIR}"
  DEPENDS "${CMAKE_SOURCE_DIR}/gui/2dcurves.json" "${CMAKE_CURRENT_SOURCE_DIR}/gen_function_db.py" funkplot-builtins
  COMMENT "Generating Function Database HTML"
)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/funkplot.qhc ${CMAKE_CURRENT_BINARY_DIR}/funkplot.qch
  COMMAND ${CMAKE_COMMAND} -E make_directory ${HTML_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/img_baser.py" "${CMAKE_CURRENT_BINARY_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/html/*.html" "${HTML_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/functions.html" "${HTML_DIR}/functions.html"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/funk*" "${CMAKE_CURRENT_BINARY_DIR}"
  COMMAND python img_baser.py
  COMMAND qhelpgenerator funkplot.qhcp -o ${CMAKE_CURRENT_BINARY_DIR}/funkplot.qhc
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/img_baser.py" ${CMAKE_CURRENT_BINARY_DIR}/functions.html
  COMMENT "Generating help"
)

# Generate a QRC file to embed the generated help files
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/help.qrc
  "<RCC>\n"
  "  <qresource prefix=\"/help\">\n"
  "    <file alias=\"funkplot.qhc\">funkplot.qhc</file>\n"
  "    <file alias=\"funkplot.qch\">funkplot.qch</file>\n"
  "  </qresource>\n"
  "</RCC>\n"
)

add_custom_target(funkplot-help ALL
    ${CMAKE_COMMAND} -E echo "Build help"
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/funkplot.qhc
)

# Help is not needed since it is embedded in the resources
# INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/funkplot.qhc DESTINATION "/usr/share/funkplot/help")
# INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/funkplot.qch DESTINATION "/usr/share/funkplot/help")
