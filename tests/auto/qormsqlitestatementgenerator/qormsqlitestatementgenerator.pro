QT += testlib orm orm-private
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_sqlitestatementgenerator.cpp \
    domain/province.cpp \
    domain/town.cpp \
    domain/person.cpp \
    domain/community.cpp \
    domain/withqvariant.cpp

HEADERS += \
    domain/province.h \
    domain/town.h \
    domain/person.h \
    domain/community.h \
    domain/withqvariant.h
