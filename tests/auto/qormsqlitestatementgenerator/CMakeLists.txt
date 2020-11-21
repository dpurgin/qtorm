add_executable(tst_sqlitestatementgenerator
    tst_sqlitestatementgenerator.cpp

    domain/province.cpp
    domain/town.cpp    

    domain/province.h
    domain/town.h
)

target_link_libraries(tst_sqlitestatementgenerator
    Qt5::Test
    qtorm
)

add_test(NAME tst_sqlitestatementgenerator COMMAND tst_sqlitestatementgenerator)
