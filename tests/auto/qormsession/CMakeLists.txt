qtorm_add_unit_test(NAME tst_ormsession SOURCES
    tst_ormsession.cpp

    domain/person.cpp
    domain/province.cpp
    domain/town.cpp
    domain/withnotnull.cpp
    domain/withforeignkey.cpp
    domain/withunique.cpp
    domain/withkeyword.cpp

    domain/person.h
    domain/province.h
    domain/town.h
    domain/withnotnull.h
    domain/withforeignkey.h
    domain/withunique.h
    domain/withkeyword.h

    ormsession.qrc

    LINK_LIBRARIES Qt${QTORM_QT_VERSION_MAJOR}::Sql
)

