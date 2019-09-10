QT += orm

TARGET = navigationdb
TEMPLATE = app

SOURCES += \
    domain/province.cpp \
    main.cpp
    
target.path = $$[QT_INSTALL_EXAMPLES]/orm/navigationdb
INSTALLS += target


RESOURCES += navigationdb.qrc

HEADERS += \
    domain/province.h
