QT = core testlib orm

CONFIG += testcase warn_on silent c++17

TARGET = tst_ormentitylistmodel

SOURCES += tst_ormentitylistmodel.cpp \
    domain/province.cpp \
    domain/town.cpp \

HEADERS += \
    domain/province.h \
    domain/town.h \

RESOURCES += resource.qrc
