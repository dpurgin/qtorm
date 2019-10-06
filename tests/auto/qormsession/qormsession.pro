QT = core testlib orm

CONFIG += testcase warn_on silent c++17

TARGET = tst_ormsession

SOURCES +=  tst_ormsession.cpp \
    domain/province.cpp \
    domain/town.cpp \

HEADERS += \
    domain/province.h \
    domain/town.h \

RESOURCES += ormsession.qrc
