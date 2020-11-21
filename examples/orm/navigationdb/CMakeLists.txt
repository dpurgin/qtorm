find_package(Qt5 COMPONENTS Core Quick REQUIRED)

add_executable(navigationdb
    domain/province.h
    domain/community.h

    domain/province.cpp
    domain/community.cpp
    main.cpp

    navigationdb.qrc
)

target_link_libraries(navigationdb PUBLIC qtorm Qt5::Core Qt5::Quick)
