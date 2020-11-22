add_executable(tst_domainclasses
    tst_domainclasses.cpp

    domainclasses.qrc
)

target_link_libraries(tst_domainclasses
    Qt5::Test
    qtorm
)

add_test(NAME tst_domainclasses COMMAND tst_domainclasses)
