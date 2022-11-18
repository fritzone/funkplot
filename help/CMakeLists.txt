CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

file(GLOB HTMLS "${CMAKE_CURRENT_SOURCE_DIR}/html/*.html")

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/funkplot.qhc
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/html
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/img_baser.py" "${CMAKE_CURRENT_BINARY_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/html/*.html" "${CMAKE_CURRENT_BINARY_DIR}/html"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/funk*" "${CMAKE_CURRENT_BINARY_DIR}"
  COMMAND python img_baser.py
  COMMAND qhelpgenerator funkplot.qhcp -o ${CMAKE_CURRENT_BINARY_DIR}/funkplot.qhc
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/img_baser.py" ${HTMLS}
  COMMENT "Generating help"
)

add_custom_target(funkplot-help ALL
    ${CMAKE_COMMAND} -E echo "Build help"
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/funkplot.qhc
)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/funkplot.qhc DESTINATION "/usr/share/funkplot/help")
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/funkplot.qch DESTINATION "/usr/share/funkplot/help")
