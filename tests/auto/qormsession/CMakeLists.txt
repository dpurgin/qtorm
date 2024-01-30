qtorm_add_unit_test(NAME tst_ormsession SOURCES
    tst_ormsession.cpp

    domain/person.cpp
    domain/province.cpp
    domain/town.cpp

    domain/person.h
    domain/province.h
    domain/town.h

    ormsession.qrc

    LINK_LIBRARIES Qt5::Sql
)

