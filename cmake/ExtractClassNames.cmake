function(ExtractClassNames HEADER OUTPUT_VARIABLE)
    set(RESULT "")
    get_filename_component(OUTPUT_HEADER "${HEADER}" NAME)

    file(STRINGS ${HEADER} LINES)

    foreach (LINE ${LINES})
        if ("${LINE}" MATCHES "class[\t ]+(Q_ORM_EXPORT[\t ]+)?(QOrm[A-Za-z0-9_]+)[^;]*$")
            list(APPEND RESULT ${CMAKE_MATCH_2})
        endif()
    endforeach()

    set("${OUTPUT_VARIABLE}" ${RESULT} PARENT_SCOPE)
endfunction()
