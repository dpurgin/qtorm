find_package(Qt${QTORM_QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

if (QTORM_QT_VERSION_MAJOR EQUAL 6)
    set(RESOURCES navigationdb-qt6.qrc)
else()
    set(RESOURCES navigationdb.qrc)
endif()

add_executable(navigationdb
    domain/province.h
    domain/community.h

    domain/province.cpp
    domain/community.cpp
    main.cpp

    ${RESOURCES}
)

target_link_libraries(navigationdb PUBLIC qtorm Qt${QTORM_QT_VERSION_MAJOR}::Core Qt${QTORM_QT_VERSION_MAJOR}::Quick)
