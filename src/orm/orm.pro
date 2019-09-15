TARGET = QtOrm

QT = core sql

CONFIG += c++11

QMAKE_DOCS = $$PWD/doc/qtorm.qdocconf

PUBLIC_HEADERS += \
    qormabstractprovider.h \
    qormentitymetadata.h \
    qormerror.h \
    qormfield.h \
    qormglobal.h \
    qormorderclause.h \
    qormorderclausebuilder.h \
    qormpropertymapping.h \
    qormquery.h \
    qormquerybuilder.h \
    qormqueryresult.h \
    qormsession.h \
    qormsessionconfiguration.h \
    qormsqlconfiguration.h \
    qormsqlprovider.h \
    qormtransactiontoken.h \
    qormwhereclause.h \
    qormwhereclausebuilder.h \

#PRIVATE_HEADERS =

SOURCES += \
    qormabstractprovider.cpp \
    qormentitymetadata.cpp \
    qormerror.cpp \
    qormfield.cpp \
    qormorderclause.cpp \
    qormorderclausebuilder.cpp \
    qormpropertymapping.cpp \
    qormquery.cpp \
    qormquerybuilder.cpp \
    qormqueryresult.cpp \
    qormsession.cpp \
    qormsessionconfiguration.cpp \
    qormsqlconfiguration.cpp \
    qormsqlprovider.cpp \
    qormtransactiontoken.cpp \
    qormwhereclause.cpp \
    qormwhereclausebuilder.cpp \
    qormglobal.cpp \

HEADERS += $$PUBLIC_HEADERS \ \ \ \ \ \ \ #$$PRIVATE_HEADERS    





load(qt_module)

DEFINES -= QT_ASCII_CAST_WARNINGS
