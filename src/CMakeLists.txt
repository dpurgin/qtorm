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

GetGeneratedHeaders("${GENERATED_INCLUDE_DIRECTORY}/QtOrm"
                    "${QTORM_PUBLIC_HEADERS}"
                    "${QTORM_PRIVATE_HEADERS}"
                    QTORM_GENERATED_HEADERS)

list(JOIN QTORM_PUBLIC_HEADERS "\\;" QTORM_PUBLIC_HEADERS_JOINED)
list(JOIN QTORM_PRIVATE_HEADERS "\\;" QTORM_PRIVATE_HEADERS_JOINED)

add_custom_command(
    OUTPUT ${QTORM_GENERATED_HEADERS}
    COMMAND ${CMAKE_COMMAND}
        -DSOURCE_DIRECTORY:STRING="${CMAKE_CURRENT_SOURCE_DIR}"
        -DOUTPUT_DIRECTORY:STRING="${GENERATED_INCLUDE_DIRECTORY}/QtOrm"
        -DPUBLIC_HEADERS:STRING="${QTORM_PUBLIC_HEADERS_JOINED}"
        -DPRIVATE_HEADERS:STRING="${QTORM_PRIVATE_HEADERS_JOINED}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/GenerateHeaders.cmake"
        DEPENDS ${QTORM_PUBLIC_HEADERS} ${QTORM_PRIVATE_HEADERS}
    COMMENT "Generating module headers...")

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

set(BUILD_SHARED_LIBS ${QTORM_BUILD_SHARED_LIBS})

add_library(qtorm
    ${QTORM_PUBLIC_HEADERS}
    ${QTORM_PRIVATE_HEADERS}
    ${QTORM_SOURCES}
    ${QTORM_GENERATED_HEADERS}
)

target_link_libraries(qtorm PUBLIC Qt5::Core PRIVATE Qt5::Sql)

target_compile_definitions(qtorm PRIVATE QT_BUILD_ORM_LIB)
target_include_directories(qtorm
    PUBLIC
        "${GENERATED_INCLUDE_DIRECTORY}"
        "${GENERATED_INCLUDE_DIRECTORY}/QtOrm"
)
target_compile_features(qtorm PUBLIC cxx_std_17)
#add_dependencies(qtorm qtorm-headers)

if (MSVC)
    target_compile_definitions(qtorm PRIVATE __PRETTY_FUNCTION__=__FUNCTION__)
endif()
