function(GetGeneratedHeaders OUTPUT_DIRECTORY PUBLIC_HEADERS PRIVATE_HEADERS OUTPUT_VARIABLE)
    set(RESULT "${OUTPUT_DIRECTORY}/QtOrm")

    foreach(HEADER ${PUBLIC_HEADERS})
        get_filename_component(OUTPUT_HEADER "${HEADER}" NAME)

        list(APPEND RESULT "${OUTPUT_DIRECTORY}/${OUTPUT_HEADER}")
        ExtractClassNames(${HEADER} CLASS_NAMES)

        foreach(CLASS_NAME ${CLASS_NAMES})
            list(APPEND RESULT "${OUTPUT_DIRECTORY}/${CLASS_NAME}")
        endforeach()
    endforeach()

    foreach(HEADER ${PRIVATE_HEADERS})
        get_filename_component(OUTPUT_HEADER "${HEADER}" NAME)

        list(APPEND RESULT "${OUTPUT_DIRECTORY}/private/${OUTPUT_HEADER}")
    endforeach()

    list(REMOVE_DUPLICATES RESULT)

    set("${OUTPUT_VARIABLE}" ${RESULT} PARENT_SCOPE)
endfunction()
