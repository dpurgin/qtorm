TARGET = QtOrm

QT = core sql

QMAKE_DOCS = $$PWD/doc/qtorm.qdocconf

PUBLIC_HEADERS += \
    qormabstractprovider.h \
    qormclassproperty.h \
    qormentityinstancecache.h \
    qormerror.h \
    qormfilter.h \
    qormfilterexpression.h \
    qormglobal.h \
    qormmetadata.h \
    qormmetadatacache.h \
    qormorder.h \
    qormpropertymapping.h \
    qormquery.h \
    qormquerybuilder.h \
    qormqueryresult.h \
    qormrelation.h \
    qormsession.h \
    qormsessionconfiguration.h \
    qormsqliteconfiguration.h \
    qormsqliteprovider.h \
    qormtransactiontoken.h \

PRIVATE_HEADERS = \
    qormglobal_p.h \
    qormmetadata_p.h \
    qormsqlitestatementgenerator_p.h \

SOURCES += \
    qormabstractprovider.cpp \
    qormclassproperty.cpp \
    qormentityinstancecache.cpp \
    qormerror.cpp \
    qormfilter.cpp \
    qormfilterexpression.cpp \
    qormglobal.cpp \
    qormglobal_p.cpp \
    qormmetadata.cpp \
    qormmetadatacache.cpp \
    qormorder.cpp \
    qormpropertymapping.cpp \
    qormquery.cpp \
    qormquerybuilder.cpp \
    qormqueryresult.cpp \
    qormrelation.cpp \
    qormsession.cpp \
    qormsessionconfiguration.cpp \
    qormsqliteconfiguration.cpp \
    qormsqliteprovider.cpp \
    qormsqlitestatementgenerator_p.cpp \
    qormtransactiontoken.cpp \

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

load(qt_module)

CONFIG += c++17

DEFINES -= QT_ASCII_CAST_WARNINGS
