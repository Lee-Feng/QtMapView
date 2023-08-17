# build a Qt app
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = app
CONFIG += thread

# project path defInition
include(path.pri)

# dest dir for apps
DESTDIR = $$PTVAR_BIN
