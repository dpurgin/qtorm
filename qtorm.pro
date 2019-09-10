requires(qtHaveModule(sql))

lessThan(QT_MAJOR_VERSION, 5) {
    message("Cannot build current QtOrm sources with Qt version $${QT_VERSION}.")
}

load(configure)
load(qt_parts)
