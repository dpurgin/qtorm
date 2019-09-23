QT += testlib orm
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_metadatacachetest.cpp
