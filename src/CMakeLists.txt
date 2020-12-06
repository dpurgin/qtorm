function(GenerateHeaders OUTPUT_DIRECTORY PUBLIC_HEADERS PRIVATE_HEADERS)
    file(REMOVE_RECURSE "${OUTPUT_DIRECTORY}")

    foreach(HEADER ${PUBLIC_HEADERS})
        get_filename_component(OUTPUT_HEADER "${HEADER}" NAME)

        file(WRITE
            "${OUTPUT_DIRECTORY}/${OUTPUT_HEADER}"
            "#include \"${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}\"")

        file(STRINGS ${HEADER} LINES)

        foreach (LINE ${LINES})
            if ("${LINE}" MATCHES "class[\t ]+(Q_ORM_EXPORT[\t ]+)?(QOrm[A-Za-z0-9_]+)[^;]*$")
                file(WRITE
                    "${OUTPUT_DIRECTORY}/${CMAKE_MATCH_2}"
                    "#include \"${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}\"")
            endif()
        endforeach()
    endforeach()

    foreach(HEADER ${PRIVATE_HEADERS})
        get_filename_component(OUTPUT_HEADER "${HEADER}" NAME)

        file(WRITE
            "${OUTPUT_DIRECTORY}/private/${OUTPUT_HEADER}"
            "#include \"${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}\"")
    endforeach()
endfunction()

set(QTORM_PUBLIC_HEADERS
    orm/qormabstractprovider.h
    orm/qormclassproperty.h
    orm/qormentityinstancecache.h
    orm/qormentitylistmodel.h
    orm/qormerror.h
    orm/qormfilter.h
    orm/qormfilterexpression.h
    orm/qormglobal.h
    orm/qormmetadata.h
    orm/qormmetadatacache.h
    orm/qormorder.h
    orm/qormpropertymapping.h
    orm/qormquery.h
    orm/qormquerybuilder.h
    orm/qormqueryresult.h
    orm/qormrelation.h
    orm/qormsession.h
    orm/qormsessionconfiguration.h
    orm/qormsqliteconfiguration.h
    orm/qormsqliteprovider.h
    orm/qormtransactiontoken.h
)

set(QTORM_PRIVATE_HEADERS
    orm/qormglobal_p.h
    orm/qormmetadata_p.h
    orm/qormsqlitestatementgenerator_p.h
)

set(GENERATED_INCLUDE_DIRECTORY "${CMAKE_BINARY_DIR}/QtOrmGenerated/include")
GenerateHeaders("${GENERATED_INCLUDE_DIRECTORY}/QtOrm"
                "${QTORM_PUBLIC_HEADERS}"
                "${QTORM_PRIVATE_HEADERS}")

set(QTORM_SOURCES
    orm/qormabstractprovider.cpp
    orm/qormclassproperty.cpp
    orm/qormentityinstancecache.cpp
    orm/qormentitylistmodel.cpp
    orm/qormerror.cpp
    orm/qormfilter.cpp
    orm/qormfilterexpression.cpp
    orm/qormglobal.cpp
    orm/qormglobal_p.cpp
    orm/qormmetadata.cpp
    orm/qormmetadatacache.cpp
    orm/qormorder.cpp
    orm/qormpropertymapping.cpp
    orm/qormquery.cpp
    orm/qormquerybuilder.cpp
    orm/qormqueryresult.cpp
    orm/qormrelation.cpp
    orm/qormsession.cpp
    orm/qormsessionconfiguration.cpp
    orm/qormsqliteconfiguration.cpp
    orm/qormsqliteprovider.cpp
    orm/qormsqlitestatementgenerator_p.cpp
    orm/qormtransactiontoken.cpp
)

if (QTORM_BUILD_SHARED_LIBS)
    add_library(qtorm SHARED ${QTORM_PUBLIC_HEADERS} ${QTORM_PRIVATE_HEADERS} ${QTORM_SOURCES})
else()
    add_library(qtorm STATIC ${QTORM_PUBLIC_HEADERS} ${QTORM_PRIVATE_HEADERS} ${QTORM_SOURCES})
endif()

target_link_libraries(qtorm PUBLIC Qt5::Core PRIVATE Qt5::Sql)

target_compile_definitions(qtorm PRIVATE QT_BUILD_ORM_LIB)
target_include_directories(qtorm
    PUBLIC
        "${GENERATED_INCLUDE_DIRECTORY}"
        "${GENERATED_INCLUDE_DIRECTORY}/QtOrm"
)
target_compile_features(qtorm PUBLIC cxx_std_17)

if (MSVC)
    target_compile_definitions(qtorm PRIVATE __PRETTY_FUNCTION__=__FUNCTION__)
endif()
