cmake_minimum_required(VERSION 3.10)

set(PROJECT funkplot)
project(${PROJECT})

set(${PROJECT}_srcs
	${CMAKE_CURRENT_SOURCE_DIR}/AbstractDrawer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AbstractDrawer.h
	${CMAKE_CURRENT_SOURCE_DIR}/Append.h
	${CMAKE_CURRENT_SOURCE_DIR}/Append.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Assignment.h
	${CMAKE_CURRENT_SOURCE_DIR}/Assignment.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ArrayAssignment.h
	${CMAKE_CURRENT_SOURCE_DIR}/ArrayAssignment.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ArithmeticAssignment.h
	${CMAKE_CURRENT_SOURCE_DIR}/ArithmeticAssignment.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ArithmeticAssignmentToArrayElement.h
	${CMAKE_CURRENT_SOURCE_DIR}/ArithmeticAssignmentToArrayElement.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ArrayIteratorLoopTarget.h
	${CMAKE_CURRENT_SOURCE_DIR}/ArrayIteratorLoopTarget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Function.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Function.h
	${CMAKE_CURRENT_SOURCE_DIR}/CodeEngine.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CodeEngine.h
	${CMAKE_CURRENT_SOURCE_DIR}/constants.h
	${CMAKE_CURRENT_SOURCE_DIR}/Done.h
	${CMAKE_CURRENT_SOURCE_DIR}/Done.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DrawnLine.h
	${CMAKE_CURRENT_SOURCE_DIR}/DrawnPoint.h
	${CMAKE_CURRENT_SOURCE_DIR}/Else.h
	${CMAKE_CURRENT_SOURCE_DIR}/Else.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Executor.h
	${CMAKE_CURRENT_SOURCE_DIR}/Executor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/FunctionDefinition.h
	${CMAKE_CURRENT_SOURCE_DIR}/FunctionDefinition.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/IndexedAccess.h
	${CMAKE_CURRENT_SOURCE_DIR}/ImageDrawer.h
	${CMAKE_CURRENT_SOURCE_DIR}/FunctionIteratorLoopTarget.h
	${CMAKE_CURRENT_SOURCE_DIR}/FunctionIteratorLoopTarget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Keywords.h
	${CMAKE_CURRENT_SOURCE_DIR}/Loop.h
	${CMAKE_CURRENT_SOURCE_DIR}/Loop.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/If.h
	${CMAKE_CURRENT_SOURCE_DIR}/If.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ImageDrawer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/LoopTarget.h
	${CMAKE_CURRENT_SOURCE_DIR}/Plot.h
	${CMAKE_CURRENT_SOURCE_DIR}/Plot.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Polar.h
	${CMAKE_CURRENT_SOURCE_DIR}/Polar.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Statement.h
	${CMAKE_CURRENT_SOURCE_DIR}/Statement.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/StatementHandler.h
	${CMAKE_CURRENT_SOURCE_DIR}/StatementHandler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Stepped.h
	${CMAKE_CURRENT_SOURCE_DIR}/Stepped.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Set.h
	${CMAKE_CURRENT_SOURCE_DIR}/Set.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parametric.h
	${CMAKE_CURRENT_SOURCE_DIR}/Parametric.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PointArrayAssignment.h
	${CMAKE_CURRENT_SOURCE_DIR}/PointArrayAssignment.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PointsOfObjectAssignment.h
	${CMAKE_CURRENT_SOURCE_DIR}/PointsOfObjectAssignment.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PointDefinitionAssignment.h
	${CMAKE_CURRENT_SOURCE_DIR}/PointDefinitionAssignment.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PointDefinitionAssignmentToOtherPoint.h
	${CMAKE_CURRENT_SOURCE_DIR}/PointDefinitionAssignmentToOtherPoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Rotation.h
	${CMAKE_CURRENT_SOURCE_DIR}/Rotation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RangeIteratorLoopTarget.h
	${CMAKE_CURRENT_SOURCE_DIR}/RangeIteratorLoopTarget.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RuntimeProvider.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RuntimeProvider.h
	${CMAKE_CURRENT_SOURCE_DIR}/util.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/util.h
	${CMAKE_CURRENT_SOURCE_DIR}/VariableDeclaration.h
	${CMAKE_CURRENT_SOURCE_DIR}/VariableDeclaration.cpp
)

# If we want python script support
if(ENABLE_PYTHON)

	find_package (Python3
		COMPONENTS Interpreter Development
		REQUIRED
	)

	list(APPEND ${PROJECT}_srcs
		${CMAKE_CURRENT_SOURCE_DIR}/python/pyembed.h 
		${CMAKE_CURRENT_SOURCE_DIR}/python/pyembed.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/python/PythonRunner.h 
		${CMAKE_CURRENT_SOURCE_DIR}/python/PythonRunner.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/python/PythonScript.h
		${CMAKE_CURRENT_SOURCE_DIR}/python/PythonScript.cpp
	)

endif()

add_library(${PROJECT} STATIC
	${${PROJECT}_srcs}
)

if(ENABLE_PYTHON)

    target_include_directories(${PROJECT} 
	PRIVATE 
    	${CMAKE_CURRENT_SOURCE_DIR}/python 		
    	${Python3_INCLUDE_DIRS}
	)

	target_link_libraries(${PROJECT}
	    Qt${QT_VERSION_MAJOR}::Core
		Qt${QT_VERSION_MAJOR}::Gui
		${Python3_LIBRARIES}
    )

   	target_compile_definitions(${PROJECT} PRIVATE "ENABLE_PYTHON")
  	message("Python at:${Python3_LIBRARIES}")


else()

	target_link_libraries(${PROJECT}
	    Qt${QT_VERSION_MAJOR}::Core
		Qt${QT_VERSION_MAJOR}::Gui
	)
endif()
