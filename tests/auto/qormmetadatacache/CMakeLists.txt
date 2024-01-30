qtorm_add_unit_test(NAME tst_metadatacachetest SOURCES
    tst_metadatacachetest.cpp

    domain/person.cpp
    domain/province.cpp
    domain/town.cpp
    domain/withenum.cpp

    domain/person.h
    domain/province.h
    domain/town.h
    domain/withenum.h
)