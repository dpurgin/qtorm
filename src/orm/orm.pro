TARGET = QtOrm

QT = core sql

QMAKE_DOCS = $$PWD/doc/qtorm.qdocconf

PUBLIC_HEADERS += \
    qormabstractprovider.h \
    qormclassproperty.h \
    qormentitymetadata.h \
    qormerror.h \
    qormfilter.h \
    qormfilterexpression.h \
    qormglobal.h \
    qormorder.h \
    qormorderbuilder.h \
    qormpropertymapping.h \
    qormquery.h \
    qormquerybuilder.h \
    qormqueryresult.h \
    qormsession.h \
    qormsessionconfiguration.h \
    qormsqlconfiguration.h \
    qormsqlprovider.h \
    qormtransactiontoken.h \

#PRIVATE_HEADERS =

SOURCES += \
    qormabstractprovider.cpp \
    qormclassproperty.cpp \
    qormentitymetadata.cpp \
    qormerror.cpp \
    qormfilter.cpp \
    qormfilterexpression.cpp \
    qormorder.cpp \
    qormorderbuilder.cpp \
    qormpropertymapping.cpp \
    qormquery.cpp \
    qormquerybuilder.cpp \
    qormqueryresult.cpp \
    qormsession.cpp \
    qormsessionconfiguration.cpp \
    qormsqlconfiguration.cpp \
    qormsqlprovider.cpp \
    qormtransactiontoken.cpp \
    qormglobal.cpp \

HEADERS += $$PUBLIC_HEADERS \ \ \ \ \ \ \ #$$PRIVATE_HEADERS    

load(qt_module)

CONFIG += c++17

DEFINES -= QT_ASCII_CAST_WARNINGS
