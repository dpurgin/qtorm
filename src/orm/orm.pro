TARGET = QtOrm

QT = core sql

QMAKE_DOCS = $$PWD/doc/qtorm.qdocconf

PUBLIC_HEADERS += \
    qormabstractprovider.h \
    qormclassproperty.h \
    qormmetadata.h \
    qormmetadatacache.h \
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
    qormsqliteprovider.h \
    qormtransactiontoken.h \

PRIVATE_HEADERS = \
    qormsqlitestatementgenerator_p.h \
    qormglobal_p.h

SOURCES += \
    qormabstractprovider.cpp \
    qormclassproperty.cpp \
    qormerror.cpp \
    qormfilter.cpp \
    qormfilterexpression.cpp \
    qormmetadata.cpp \
    qormmetadatacache.cpp \
    qormorder.cpp \
    qormorderbuilder.cpp \
    qormpropertymapping.cpp \
    qormquery.cpp \
    qormquerybuilder.cpp \
    qormqueryresult.cpp \
    qormsession.cpp \
    qormsessionconfiguration.cpp \
    qormsqlconfiguration.cpp \
    qormsqlitestatementgenerator_p.cpp \
    qormsqliteprovider.cpp \
    qormtransactiontoken.cpp \
    qormglobal.cpp \

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

load(qt_module)

CONFIG += c++17

DEFINES -= QT_ASCII_CAST_WARNINGS
