add_executable(tst_ormsession
    tst_ormsession.cpp

    domain/person.cpp
    domain/province.cpp
    domain/town.cpp

    domain/person.h
    domain/province.h
    domain/town.h

    ormsession.qrc
)

target_link_libraries(tst_ormsession
    Qt5::Test
    Qt5::Sql
    qtorm
)

add_test(NAME tst_ormsession COMMAND tst_ormsession)
