include(${CMAKE_CURRENT_LIST_DIR}/ExtractClassNames.cmake)

function(CheckVariable VARIABLE_NAME)
	if (NOT DEFINED ${VARIABLE_NAME})
		message(FATAL_ERROR "${VARIABLE_NAME} is not defined")
	else() 
		message(STATUS "${VARIABLE_NAME} is ${${VARIABLE_NAME}}")
	endif()
endfunction()

CheckVariable(SOURCE_DIRECTORY)
CheckVariable(OUTPUT_DIRECTORY)
CheckVariable(PUBLIC_HEADERS)
CheckVariable(PRIVATE_HEADERS)

file(REMOVE_RECURSE "${OUTPUT_DIRECTORY}")

set(QTORM_MODULE_HEADER "${OUTPUT_DIRECTORY}/QtOrm")
file(WRITE  ${QTORM_MODULE_HEADER} "#ifndef QT_QTORM_MODULE_H\n")
file(APPEND ${QTORM_MODULE_HEADER} "#define QT_QTORM_MODULE_H\n")

foreach(HEADER ${PUBLIC_HEADERS})
	message(STATUS "Parsing ${HEADER}...")
	
	get_filename_component(OUTPUT_HEADER "${HEADER}" NAME)

	file(WRITE
		"${OUTPUT_DIRECTORY}/${OUTPUT_HEADER}"
		"#include \"${SOURCE_DIRECTORY}/${HEADER}\"")

	ExtractClassNames(${SOURCE_DIRECTORY}/${HEADER} CLASS_NAMES)

	foreach(CLASS_NAME ${CLASS_NAMES})
		file(WRITE
			"${OUTPUT_DIRECTORY}/${CLASS_NAME}"
			"#include \"${SOURCE_DIRECTORY}/${HEADER}\"")
	endforeach()

	file(APPEND "${QTORM_MODULE_HEADER}" "#include \"${OUTPUT_HEADER}\"\n")
endforeach()

foreach(HEADER ${PRIVATE_HEADERS})
	get_filename_component(OUTPUT_HEADER "${HEADER}" NAME)

	file(WRITE
		"${OUTPUT_DIRECTORY}/private/${OUTPUT_HEADER}"
		"#include \"${SOURCE_DIRECTORY}/${HEADER}\"")
endforeach()

file(APPEND ${QTORM_MODULE_HEADER} "#endif\n")

