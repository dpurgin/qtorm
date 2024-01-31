QT += testlib orm
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_metadatacachetest.cpp \
    domain/person.cpp \
    domain/province.cpp \
    domain/town.cpp \
    domain/withenum.cpp 

HEADERS += \
    domain/person.h \
    domain/province.h \
    domain/town.h \
    domain/withenum.h 

