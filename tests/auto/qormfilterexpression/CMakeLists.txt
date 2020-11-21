add_executable(tst_ormfilterexpression
    tst_ormfilterexpression.cpp
)

target_link_libraries(tst_ormfilterexpression
    Qt5::Test
    qtorm
)

add_test(NAME tst_ormfilterexpression COMMAND tst_ormfilterexpression)
