QT = core testlib orm orm-private

CONFIG += testcase warn_on silent c++17

TARGET = tst_ormsession

SOURCES +=  tst_ormsession.cpp \
    domain/province.cpp \
    domain/town.cpp \
    domain/person.cpp \
    domain/withnotnull.cpp \
    domain/withforeignkey.cpp \
    domain/withunique.cpp \
    domain/withkeyword.cpp \

HEADERS += \
    domain/province.h \
    domain/town.h \
    domain/person.h \
    domain/withnotnull.h \
    domain/withforeignkey.h \
    domain/withunique.h \
    domain/withkeyword.h \

RESOURCES += ormsession.qrc
