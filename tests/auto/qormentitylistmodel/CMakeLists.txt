add_executable(tst_ormentitylistmodel
    tst_qormentitylistmodel.cpp

    domain/province.cpp
    domain/town.cpp

    domain/province.h
    domain/town.h

    resource.qrc
)

target_link_libraries(tst_ormentitylistmodel
    Qt5::Test
    qtorm
)

add_test(NAME tst_ormentitylistmodel COMMAND tst_ormentitylistmodel)
