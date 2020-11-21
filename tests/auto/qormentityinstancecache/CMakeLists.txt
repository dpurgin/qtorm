add_executable(tst_entityinstancecache
    tst_entityinstancecache.cpp
    domain/province.cpp
    domain/town.cpp

    domain/province.h
    domain/town.h
)

target_link_libraries(tst_entityinstancecache
    Qt5::Test
    qtorm
)

add_test(NAME tst_entityinstancecache COMMAND tst_entityinstancecache)
