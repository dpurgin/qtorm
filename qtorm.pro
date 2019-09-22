requires(qtHaveModule(sql))

lessThan(QT_MAJOR_VERSION, 5) {
    message("Cannot build current QtOrm sources with Qt version $${QT_VERSION}.")
}

QTORM_BUILD_PARTS = libs tests examples

load(configure)
load(qt_parts)
