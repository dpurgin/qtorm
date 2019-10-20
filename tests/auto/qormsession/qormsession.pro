QT = core testlib orm orm-private

CONFIG += testcase warn_on silent c++17

TARGET = tst_ormsession

SOURCES +=  tst_ormsession.cpp \
    domain/province.cpp \
    domain/town.cpp \
    domain/person.cpp \

HEADERS += \
    domain/province.h \
    domain/town.h \
    domain/person.h \

RESOURCES += ormsession.qrc
