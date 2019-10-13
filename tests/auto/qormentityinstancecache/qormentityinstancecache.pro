QT += testlib orm
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_entityinstancecache.cpp \
    domain/province.cpp \
    domain/town.cpp

HEADERS += \
    domain/province.h \
    domain/town.h
