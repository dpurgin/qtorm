add_executable(tst_sqlitestatementgenerator
    tst_sqlitestatementgenerator.cpp

    domain/province.h
    domain/province.cpp

    domain/town.h
    domain/town.cpp            

    domain/person.h
    domain/person.cpp

    domain/community.h
    domain/community.cpp

    domain/withqvariant.h
    domain/withqvariant.cpp
)

target_link_libraries(tst_sqlitestatementgenerator
    Qt5::Test
    qtorm
)

add_test(NAME tst_sqlitestatementgenerator COMMAND tst_sqlitestatementgenerator)
