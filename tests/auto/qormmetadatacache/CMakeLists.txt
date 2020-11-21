add_executable(tst_metadatacachetest
    tst_metadatacachetest.cpp

    domain/person.cpp
    domain/province.cpp
    domain/town.cpp    

    domain/person.h
    domain/province.h
    domain/town.h
)

target_link_libraries(tst_metadatacachetest
    Qt5::Test
    qtorm
)

add_test(NAME tst_metadatacachetest COMMAND tst_metadatacachetest)
