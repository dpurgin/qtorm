QT += orm  # enable the ORM module

CONFIG += c++17 # required by QtOrm

TARGET = navigationdb
TEMPLATE = app

HEADERS += \
    domain/province.h \
    domain/community.h \

SOURCES += \
    domain/province.cpp \
    domain/community.cpp \
    main.cpp
    
target.path = $$[QT_INSTALL_EXAMPLES]/orm/navigationdb
INSTALLS += target


RESOURCES += navigationdb.qrc

