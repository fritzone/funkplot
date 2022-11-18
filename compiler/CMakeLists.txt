set(PROJECT funkplot-compiler)
project(${PROJECT})

add_executable(funkplot-compiler
	${CMAKE_CURRENT_SOURCE_DIR}/compiler.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/ImageDrawer.h
	${CMAKE_CURRENT_SOURCE_DIR}/ImageDrawer.cpp

)

target_include_directories(${PROJECT} PRIVATE ${CMAKE_SOURCE_DIR}/lib)

target_link_libraries(funkplot-compiler
	PRIVATE
	funkplot
	${QtLibList}
)
